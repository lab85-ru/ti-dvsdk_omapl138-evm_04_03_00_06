/** ============================================================================
 *  @file   pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the POOL API Functions.
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

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drv_api.h>
#include <drvdefs.h>

/*  ----------------------------------- API Headers                 */
#include <proc.h>
#include <_proc.h>
#include <_pool.h>
#include <_pooldefs.h>
#include <pooldefs.h>


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

/** ============================================================================
 *  @func   POOL_addrConfig
 *
 *  @desc   Pool driver specific configuration data.
 *  ============================================================================
 */
extern POOL_AddrInfo POOL_addrConfig [MAX_DSPS][MAX_POOLENTRIES] ;

/** ============================================================================
 *  @name   PROC_stateObj
 *
 *  @desc   Extern declaration of the state object for the PROC user-level
 *          component containing internal state information required by it.
 *  ============================================================================
 */
extern PROC_Object PROC_stateObj ;


/** ============================================================================
 *  @func   POOL_open
 *
 *  @desc   This function opens a specific pool referenced by the pool Id
 *          provided.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_open (IN PoolId poolId, IN Pvoid params)
{
    DSP_STATUS       status      = DSP_SOK ;
    DSP_STATUS       tmpStatus   = DSP_SOK ;
    Bool             csObjExists = FALSE   ;
    Uint8            poolNo      = POOL_getPoolNo (poolId) ;
    ProcessorId      procId      = POOL_getProcId (poolId) ;
    CMD_Args         args                  ;
    POOL_OpenParams  poolOpenParams        ;

    TRC_2ENTER ("POOL_open", poolId, params) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;

    if (!IS_VALID_POOLID (poolId)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (PROC_stateObj.syncCsObj != NULL) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            csObjExists = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (    DRV_CHECK_CURSTATUS (
                          PROC_stateObj.curStatus.poolIsOpened [procId][poolNo])
                ==  TRUE) {
                /* Check if the specific POOL has been opened in this process.*/
                status = DSP_EALREADYOPENED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.poolOpenArgs.poolId  = poolId ;
                poolOpenParams.params             = params ;
                args.apiArgs.poolOpenArgs.params  = &poolOpenParams ;

                status = DRV_INVOKE (DRV_handle, CMD_POOL_OPEN, &args) ;
                if (DSP_SUCCEEDED (status)) {
                    DRV_SET_CURSTATUS (
                        PROC_stateObj.curStatus.poolIsOpened [procId][poolNo]) ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            if (csObjExists == TRUE) {
                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("POOL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_close
 *
 *  @desc   This function closes a specific pool whose pool id is provided.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_close (IN PoolId poolId)
{
    DSP_STATUS  status      = DSP_SOK ;
    DSP_STATUS  tmpStatus   = DSP_SOK ;
    Bool        csObjExists = FALSE ;
    Uint8       poolNo      = POOL_getPoolNo (poolId) ;
    ProcessorId procId      = POOL_getProcId (poolId) ;
    CMD_Args    args ;

    TRC_1ENTER ("POOL_close", poolId) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;

    if ((!IS_VALID_POOLID (poolId))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (PROC_stateObj.syncCsObj != NULL) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            csObjExists = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (    DRV_CHECK_CURSTATUS (
                          PROC_stateObj.curStatus.poolIsOpened [procId][poolNo])
                ==  FALSE) {
                /* Check if the specific POOL has been opened in this process.*/
                status = DSP_EACCESSDENIED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.poolCloseArgs.poolId = poolId ;

                status = DRV_INVOKE (DRV_handle, CMD_POOL_CLOSE, &args) ;
                DRV_RESET_CURSTATUS (
                        PROC_stateObj.curStatus.poolIsOpened [procId][poolNo]) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (csObjExists == TRUE) {
                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("POOL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_alloc
 *
 *  @desc   This function allocates a buffer of the specified size from a pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_alloc (IN PoolId poolId, OUT Pvoid * bufPtr, IN Uint32 size)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args ;

    TRC_3ENTER ("POOL_alloc", poolId, bufPtr, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (bufPtr != NULL) ;
    DBC_Require (size   > 0) ;

    if ((!(IS_VALID_POOLID (poolId))) || (bufPtr == NULL) || (size <= 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.poolAllocArgs.poolId = poolId ;
        args.apiArgs.poolAllocArgs.bufPtr = bufPtr ;
        args.apiArgs.poolAllocArgs.size   = size ;

        status = DRV_INVOKE (DRV_handle, CMD_POOL_ALLOC, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("POOL_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_free
 *
 *  @desc   This function frees a buffer into the specified pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_free (IN PoolId poolId, IN Pvoid buf, IN Uint32 size)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args ;

    TRC_3ENTER ("POOL_free", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf  != NULL) ;
    DBC_Require (size > 0) ;

    if ((!(IS_VALID_POOLID (poolId))) || (buf == NULL) || (size <= 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.poolFreeArgs.poolId = poolId ;
        args.apiArgs.poolFreeArgs.bufPtr = buf ;
        args.apiArgs.poolFreeArgs.size   = size ;

        status = DRV_INVOKE (DRV_handle, CMD_POOL_FREE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("POOL_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_translateAddr
 *
 *  @desc   This function translates addresses between two address spaces for a
 *          buffer that was allocated from the pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_translateAddr (IN  PoolId           poolId,
                    OUT Pvoid *          dstAddr,
                    IN  AddrType         dstAddrType,
                    IN  Pvoid            srcAddr,
                    IN  AddrType         srcAddrType)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args ;

    TRC_5ENTER ("POOL_translateAddr",
                poolId,
                dstAddr,
                dstAddrType,
                srcAddr,
                srcAddrType) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (dstAddr  != NULL) ;
    DBC_Require (   (dstAddrType == AddrType_Usr)
                 || (dstAddrType == AddrType_Phy)
                 || (dstAddrType == AddrType_Knl)
                 || (dstAddrType == AddrType_Dsp)) ;
    DBC_Require (srcAddr  != NULL) ;
    DBC_Require (   (srcAddrType == AddrType_Usr)
                 || (srcAddrType == AddrType_Phy)
                 || (srcAddrType == AddrType_Knl)
                 || (srcAddrType == AddrType_Dsp)) ;

    if (    (!(IS_VALID_POOLID (poolId)))
        ||  (dstAddr == NULL)
        ||  (   (dstAddrType != AddrType_Usr)
             && (dstAddrType != AddrType_Phy)
             && (dstAddrType != AddrType_Knl)
             && (dstAddrType != AddrType_Dsp))
        ||  (srcAddr == NULL)
        ||  (   (srcAddrType != AddrType_Usr)
             && (srcAddrType != AddrType_Phy)
             && (srcAddrType != AddrType_Knl)
             && (srcAddrType != AddrType_Dsp))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.poolTranslateAddrArgs.poolId      = poolId ;
        args.apiArgs.poolTranslateAddrArgs.dstAddr     = dstAddr ;
        args.apiArgs.poolTranslateAddrArgs.dstAddrType = dstAddrType ;
        args.apiArgs.poolTranslateAddrArgs.srcAddr     = srcAddr ;
        args.apiArgs.poolTranslateAddrArgs.srcAddrType = srcAddrType ;

        status = DRV_INVOKE (DRV_handle, CMD_POOL_TRANSLATEADDR, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("POOL_translateAddr", status) ;

    return status ;
}


/** ============================================================================
 *  @func   POOL_getPoolId
 *
 *  @desc   This function gets the pool id of the buffer that was allocated from
 *          the pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
POOL_getPoolId (IN  ProcessorId procId,
                IN  Pvoid       addr,
                IN  AddrType    addrType,
                OUT PoolId *    poolId)
{
    DSP_STATUS status     = DSP_ENOTFOUND ;
    Uint32     fmAddrBase = (Uint32) NULL ;
    Uint32     size ;
    Uint32     bufPtr ;
    Uint16     j ;

    TRC_3ENTER ("POOL_getPoolId",
                addr,
                addrType,
                poolId) ;

    DBC_Require (addr   != NULL) ;
    DBC_Require (poolId != NULL) ;
    DBC_Require (   (addrType == AddrType_Usr)
                 || (addrType == AddrType_Phy)
                 || (addrType == AddrType_Knl)
                 || (addrType == AddrType_Dsp)) ;

    if (   (addr   == NULL)
        || (poolId == NULL)
        || (   (addrType != AddrType_Usr)
            && (addrType != AddrType_Phy)
            && (addrType != AddrType_Knl)
            && (addrType != AddrType_Dsp))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *poolId = POOL_INVALIDID ;
        bufPtr = (Uint32) (addr) ;

        for (j = 0 ; j < MAX_POOLENTRIES ; j++) {
            /* Perform check only if pool entry is configured. */
            if (   (POOL_addrConfig [procId][j].addr [AddrType_Usr])
                != ((Uint32) NULL)) {
                size = POOL_addrConfig [procId][j].size ;
                fmAddrBase = POOL_addrConfig [procId][j].addr [addrType] ;

                /* Check whether given address lies in current pool entry */
                if (   (bufPtr >= fmAddrBase)
                    && (bufPtr < (fmAddrBase + size))) {
                    *poolId = ((procId << 8u) & 0xFF00u) | (j & 0xFFu) ;
                    status = DSP_SOK ;
                    break ;
                }
            }
        }
    }

    TRC_1LEAVE ("POOL_getPoolId", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
