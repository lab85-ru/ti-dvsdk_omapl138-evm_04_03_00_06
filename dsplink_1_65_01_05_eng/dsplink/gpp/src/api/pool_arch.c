/** ============================================================================
 *  @file   pool_arch.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the additional POOL API Functions (based on architeecture).
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
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drv_api.h>
#include <drvdefs.h>

/*  ----------------------------------- C Libraries                 */
#include <string.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_POOL

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_POOL, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/*  ============================================================================
 *  @const  POOLNAME
 *
 *  @desc   Defined for Pool name.
 *  ============================================================================
 */
#define POOLNAME        "DMAPOOL"

/** ============================================================================
 *  @name   PROC_linkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver on the user-side.
 *  ============================================================================
 */
extern LINKCFG_Object * PROC_linkCfgPtr ;


/** ============================================================================
 *  @func   POOL_writeback
 *
 *  @desc   This function writebacks the content of a buffer.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_writeback (IN PoolId poolId, IN Pvoid buf, IN Uint32 size)
{
    CMD_Args            args                ;
    LINKCFG_Dsp *       dspObj              ;
    LINKCFG_LinkDrv *   linkDrv             ;
    LINKCFG_Pool *      poolObject          ;
    LINKCFG_DspConfig * dspCfg              ;
    ProcessorId         procId              ;
    Uint8               poolNo              ;
    DSP_STATUS          status    = DSP_SOK ;


    TRC_3ENTER ("POOL_writeback", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf  != NULL) ;
    DBC_Require (size > 0) ;

    if ((!IS_VALID_POOLID (poolId)) || (buf == NULL) || (size <= 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        procId    = POOL_getProcId (poolId) ;
        poolNo    = POOL_getPoolNo (poolId) ;
        dspCfg     = PROC_linkCfgPtr->dspConfigs [procId] ;
        dspObj     = dspCfg->dspObject ;
        linkDrv    = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        poolObject = &(dspCfg->poolTables [linkDrv->poolTableId][poolNo]) ;

        if (strcmp (poolObject->name, POOLNAME) == 0 ) {
            args.apiArgs.poolWBArgs.poolId = poolId ;
            args.apiArgs.poolWBArgs.bufPtr = buf ;
            args.apiArgs.poolWBArgs.size   = size ;

            status = DRV_INVOKE (DRV_handle, CMD_POOL_WRITEBACK, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }
    TRC_1LEAVE ("POOL_writeback", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_invalidate
 *
 *  @desc   This function invalidates the content of the buffer.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_invalidate (IN PoolId poolId, IN Pvoid buf, IN Uint32 size)
{
    CMD_Args            args                ;
    LINKCFG_Dsp *       dspObj              ;
    LINKCFG_LinkDrv *   linkDrv             ;
    LINKCFG_Pool *      poolObject          ;
    LINKCFG_DspConfig * dspCfg              ;
    ProcessorId         procId              ;
    Uint8               poolNo              ;
    DSP_STATUS          status    = DSP_SOK ;

    TRC_3ENTER ("POOL_invalidate", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf  != NULL) ;
    DBC_Require (size > 0) ;

    if ((!IS_VALID_POOLID (poolId)) || (buf == NULL) || (size <= 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        procId    = POOL_getProcId (poolId) ;
        poolNo    = POOL_getPoolNo (poolId) ;
        dspCfg     = PROC_linkCfgPtr->dspConfigs [procId] ;
        dspObj     = dspCfg->dspObject ;
        linkDrv    = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        poolObject = &(dspCfg->poolTables [linkDrv->poolTableId][poolNo]) ;

        if (strcmp (poolObject->name, POOLNAME) == 0 ) {
            args.apiArgs.poolInvArgs.poolId = poolId ;
            args.apiArgs.poolInvArgs.bufPtr = buf ;
            args.apiArgs.poolInvArgs.size   = size ;

            status = DRV_INVOKE (DRV_handle, CMD_POOL_INVALIDATE, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("POOL_invalidate", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
