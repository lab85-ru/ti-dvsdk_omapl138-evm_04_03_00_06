/** ============================================================================
 *  @file   _idm_usr.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Implementation of functions for ID Manager component on the
 *          user-side.
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

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <_idm_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_IDM


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_IDM, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   _IDM_USR_init
 *
 *  @desc   Initializes the IDM component
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_init (Void)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_0ENTER ("_IDM_USR_init") ;

    status = DRV_INVOKE (DRV_handle, CMD_IDM_INIT, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_IDM_USR_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _IDM_USR_exit
 *
 *  @desc   Finalizes the IDM component
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_exit (Void)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_0ENTER ("_IDM_USR_exit") ;

    status = DRV_INVOKE (DRV_handle, CMD_IDM_EXIT, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_IDM_USR_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _IDM_USR_create
 *
 *  @desc   Creates an IDM object identified based on a unique key specified by
 *          the user.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_create (IN Uint32 key, IN IDM_Attrs * attrs)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("_IDM_USR_create", key, attrs) ;

    DBC_Require (attrs != NULL) ;

    if (attrs == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.idmCreateArgs.key   = key ;
        args.apiArgs.idmCreateArgs.attrs = attrs ;

        status = DRV_INVOKE (DRV_handle, CMD_IDM_CREATE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("_IDM_USR_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _IDM_USR_delete
 *
 *  @desc   Deletes an IDM object identified based on a unique key specified by
 *          the user.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_delete (IN Uint32 key)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_1ENTER ("_IDM_USR_delete", key) ;

    args.apiArgs.idmDeleteArgs.key = key ;

    status = DRV_INVOKE (DRV_handle, CMD_IDM_DELETE, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_IDM_USR_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _IDM_USR_acquireId
 *
 *  @desc   Acquires a free ID for the specified IDM object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_acquireId (IN Uint32 key, IN Pstr idKey, OUT Uint32 * id)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_3ENTER ("_IDM_USR_acquireId", key, idKey, id) ;

    DBC_Require (id    != NULL) ;
    DBC_Require (idKey != NULL) ;

    if ((id == NULL) || (idKey == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.idmAcquireIdArgs.key   = key ;
        args.apiArgs.idmAcquireIdArgs.idKey = idKey ;
        args.apiArgs.idmAcquireIdArgs.id    = id ;

        status = DRV_INVOKE (DRV_handle, CMD_IDM_ACQUIREID, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("_IDM_USR_acquireId", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _IDM_USR_releaseId
 *
 *  @desc   Releases the specified ID for the specified IDM object
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_IDM_USR_releaseId (IN Uint32 key, IN Uint32 id)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("_IDM_USR_releaseId", key, id) ;

    args.apiArgs.idmReleaseIdArgs.key = key ;
    args.apiArgs.idmReleaseIdArgs.id  = id ;

    status = DRV_INVOKE (DRV_handle, CMD_IDM_RELEASEID, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_IDM_USR_releaseId", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
