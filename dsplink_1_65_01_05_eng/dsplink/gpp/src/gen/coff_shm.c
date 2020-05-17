/** ============================================================================
 *  @file   coff_shm.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implements the memory-based DSP COFF loader.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_bitops.h>
#include <_safe.h>
#include <loaderdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>
#include <mem.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

#include <coff_int.h>
#include <coff.h>
#include <coff_55x.h>
#include <coff_64x.h>
#include <coff_shm.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID         ID_GEN_COFF_SHM

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define  SET_FAILURE_REASON   TRC_SetReason (status, FID_C_GEN_COFF_SHM, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;


/** ============================================================================
 *  @name   COFFSHM_Interface
 *
 *  @desc   Interface table for file-based COFF loader.
 *  ============================================================================
 */
LOADER_Interface COFFSHM_Interface = {
    &COFFSHM_init,
    &COFFSHM_exit,
    &COFF_load,
    &COFF_loadSection,
    &COFF_getSymbolAddress
} ;


/** ============================================================================
 *  @func   COFFSHM_init
 *
 *  @desc   Initializes the context object for loading a base image file or
 *          a section. This function is required to be called before any other
 *          function is called from this sub component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFFSHM_init (IN  ProcessorId      procId,
               IN  Pstr             baseImage,
               IN  LoaderInitArgs * args,
               OUT Pvoid *          objCtx)
{
    DSP_STATUS      status      = DSP_SOK ;
    KFileObject *   fileObj     = NULL    ;
    CONST Char8 *   mode        = "r"     ;
    COFF_Context *  obj         = NULL    ;


    TRC_4ENTER ("COFFSHM_init", procId, baseImage, args, objCtx) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (baseImage != NULL) ;
    DBC_Require (objCtx != NULL) ;

    /* Allocate memory for the object context. */
    status = MEM_Calloc (objCtx, sizeof (COFF_Context), MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        obj = (COFF_Context *) (*objCtx) ;
        DBC_Assert (obj != NULL) ;

        DBC_Assert (mode != NULL) ;
        DBC_Assert (args->kfileIntf != NULL) ;

        status = KFILE_Open (baseImage, mode, args->kfileIntf, &fileObj) ;
        if (DSP_SUCCEEDED (status)) {
            DBC_Assert (fileObj != NULL) ;
            obj->fileObj = fileObj ;
            obj->imageInfo.fileAddr = (Uint32) NULL ;
            obj->imageInfo.size = 0 ;
            obj->isFileBased = COFF_SHM_LOADER ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        status = COFF_init (procId, baseImage, args, *objCtx) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_FAILED (status)) {
        FREE_PTR (*objCtx) ;
        KFILE_Close (fileObj) ;
    }

    TRC_1LEAVE ("COFFSHM_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFFSHM_exit
 *
 *  @desc   Deallocates the object(s) created by DSP_STATUS function and
 *          releases the context.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFFSHM_exit (IN  Pvoid objCtx)
{
    DSP_STATUS    status    = DSP_SOK ;
    DSP_STATUS    tmpStatus = DSP_SOK ;
    COFF_Context * obj                ;

    DBC_Require (objCtx != NULL) ;

    TRC_1ENTER ("COFFSHM_exit", objCtx) ;

    obj = (COFF_Context *) objCtx ;
    if (obj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (obj->fileObj != NULL) {
            status = KFILE_Close (obj->fileObj) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            obj->fileObj = NULL  ;
        }

        tmpStatus = COFF_exit (objCtx) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = FREE_PTR (obj) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFFSHM_exit", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
