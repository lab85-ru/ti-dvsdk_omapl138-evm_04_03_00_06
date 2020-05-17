/** ============================================================================
 *  @file   no_loader.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implements the DSP Dummy Loader.
 *          This loader is used when the DSPLINK_NOLOAD_MODE and
 *          DSPLINK_NOBOOT_MODE is selected. In this mode am external entity
 *          GPP applocation or GPP Boot Loader or DSP Loader will pre load the
 *          DSP. Using COFF_load API the application will pass the entry point
 *          i.e. c_int00 to PROC_start API.
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
#include <_dsplink.h>
#include <_loaderdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>
#include <mem.h>

/*  ----------------------------------- Generic Functions           */

#include <no_loader.h>
#include <gen_utils.h>
#include <coff_64x.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID         ID_GEN_NOLOADER

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define  SET_FAILURE_REASON   TRC_SetReason (status,                        \
                                             FID_C_GEN_NOLOADER,            \
                                             __LINE__)
#else
#define SET_FAILURE_REASON  { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  MAX_SYMBOLS
 *
 *  @desc   Maximum number of symbols supported by the dummy loader.
 *  ============================================================================
 */
#define  MAX_SYMBOLS         1u

/** ============================================================================
 *  @name   NOLOADER_SymbolName
 *
 *  @desc   Enumeration of symbol names supported by the dummy loader.
 *
 *  @field  _DSPLINK_SHMBASEADDRESS
 *              DSPLINK shared memory base address.
 *  ============================================================================
 */
typedef enum {
    _DSPLINK_SHMBASEADDRESS = 0
} NOLOADER_SymbolName ;

/*  ----------------------------------------------------------------------------
 *  @name   NOLOADER_Symbol
 *
 *  @desc   This structure defines a symbol object that stores information about
 *          a symbol supported by the dummy loader.
 *
 *  @field  symName
 *              Name of the symbol.
 *  @field  symValue
 *              Value of the symbol.
 *  ----------------------------------------------------------------------------
 */
typedef struct NOLOADER_Symbol_tag {
    Char8           symName [DSP_MAX_STRLEN] ;
    Uint32          symValue ;
} NOLOADER_Symbol ;


/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;


/** ============================================================================
 *  @name   NOLOADER_Interface
 *
 *  @desc   Interface table for Static loader.
 *  ============================================================================
 */
LOADER_Interface NOLOADER_Interface = {
    &NOLOADER_init,
    &NOLOADER_exit,
    &NOLOADER_load,
    &NOLOADER_loadSection,
    &NOLOADER_getSymbolAddress
} ;

/*  ----------------------------------------------------------------------------
 *  @name   NOLOADER_symArray
 *
 *  @desc   Array of symbol name and value pairs supported by the dummy loader.
 *  ----------------------------------------------------------------------------
 */
STATIC NOLOADER_Symbol NOLOADER_symArray [MAX_SYMBOLS] ;


/** ============================================================================
 *  @func   NOLOADER_init
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
NOLOADER_init (IN  ProcessorId      procId,
               IN  Pstr             baseImage,
               IN  LoaderInitArgs * args,
               OUT Pvoid *          objCtx)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("NOLOADER_init", procId, baseImage, args, objCtx) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (baseImage != NULL) ;
    DBC_Require (args      != NULL) ;
    DBC_Require (objCtx    != NULL) ;

    *objCtx = (Pvoid) baseImage ;


    TRC_1LEAVE ("NOLOADER_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOLOADER_exit
 *
 *  @desc   Deallocates the object(s) created by NOLOADER_init function
 *          and releases the context.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_exit (IN  Pvoid objCtx)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("NOLOADER_exit", objCtx) ;

    /* Nothing to be done to finalize the Static Loader. */

    TRC_1LEAVE ("NOLOADER_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOLOADER_load
 *
 *  @desc   Gets the entry point of the loaded DSP executable.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_load (IN  ProcessorId     procId,
               IN  LoaderObject *  loaderObj,
               IN  Uint32          argc,
               IN  Char8 **        argv,
               OUT Uint32 *        entryPt)
{
    DSP_STATUS           status  = DSP_SOK ;
    DspArch              dspArch = DspArch_C64x_Bios5 ;
    NOLOADER_ImageInfo * imageInfo ;
    Char8              * data      ;
    LINKCFG_DspConfig *  dspCfg    ;
    LINKCFG_Dsp *        dspObj    ;


    TRC_5ENTER ("NOINLOADER_load", procId, loaderObj, argc, argv, entryPt) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (loaderObj != NULL) ;
    DBC_Require (entryPt != NULL) ;
    DBC_Require (   ((argc == 0) && (argv == NULL))
                 || ((argc != 0) && (argv != NULL))) ;


    imageInfo = (NOLOADER_ImageInfo *) loaderObj->objCtx ;
    DBC_Assert (imageInfo != NULL) ;

    /* Return the address from which DSP should be run. For the dummy loader
     * this comes from the parameter
     */
    status = GEN_Strcpyn (
                      NOLOADER_symArray [_DSPLINK_SHMBASEADDRESS].symName,
                      DRV_SHMBASESYMBOL,
                      DSP_MAX_STRLEN) ;
    /* Parameters are non-NULL so success is assured. */
    DBC_Assert (DSP_SUCCEEDED (status)) ;

    NOLOADER_symArray [_DSPLINK_SHMBASEADDRESS].symValue =
                                                      imageInfo->shmBaseAddr ;
    dspCfg = LDRV_LinkCfgPtr->dspConfigs [procId] ;
    dspObj = dspCfg->dspObject ;

    /* Write the user-specified arguments into the .args section */
    if (argc > 0) {
        if (argv != NULL) {
             if (    (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
                  || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)) {

                status = MEM_Alloc ((Void **) &(data),
                                    imageInfo->argsSize,
                                    MEM_DEFAULT) ;

                /* Default indicates DSP/BIOS version is DspArch_C64x_Bios5 */
                if ( (DspArch) dspObj->dspArch == (DspArch) DspArch_C64x) {
                    dspArch = DspArch_C64x_Bios5 ;
                }

                if (DSP_SUCCEEDED (status)) {
                    status = COFF_fillArgsBuffer_64x ((DspArch) dspArch,
                                                      argc,
                                                      argv,
                                                      imageInfo->argsSize,
                                                      imageInfo->argsAddr,
                                                      loaderObj->maduSize,
                                                      loaderObj->endian,
                                                      (Void *) data) ;
                    if (DSP_SUCCEEDED (status)) {
                        status = (*(loaderObj->fnWriteDspMem))
                                                 (procId,
                                                  imageInfo->argsAddr,
                                                  loaderObj->endian,
                                                  imageInfo->argsSize,
                                                  (Uint8 *) ((Pvoid)
                                                  data)) ;
                     }
                }
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    /* Return the address from which DSP should be run. For the no loader
     * this comes from the given parameter
     */
    if (DSP_SUCCEEDED (status)) {
        *entryPt = imageInfo->dspRunAddr ;
    }

    /* ========================================================================
     * Synchronization point for Application
     * to check if pre-loading of DSP i.e. DSP executable related data
     * is complete.
     * ========================================================================
     */
    TRC_1LEAVE ("NOLOADER_load", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOLOADER_loadSection
 *
 *  @desc   Loads a section from the DSP executable onto the DSP. This is called
 *          by PMGR_PROC_loadSection through the function pointer table.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_loadSection (IN  ProcessorId     procId,
                      IN  LoaderObject *  loaderObj,
                      IN  Uint32          sectId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("NOLOADER_loadSection", procId, loaderObj, sectId) ;

    status = DSP_ENOTIMPL ;
    SET_FAILURE_REASON ;

    TRC_1LEAVE ("NOLOADER_loadSection", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOLOADER_getSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_getSymbolAddress (IN   ProcessorId  procId,
                           IN   Void *       objCtx,
                           IN   Char8 *      symbolName,
                           OUT  Uint32 *     dspAddr)
{
    DSP_STATUS status     = DSP_SOK ;
    Int32      cmpResult  = -1 ;
    Uint32     i ;

    TRC_4ENTER ("NOLOADER_getSymbolAddress",
                procId,
                objCtx,
                symbolName,
                dspAddr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (objCtx     != NULL) ;
    DBC_Require (symbolName != NULL) ;
    DBC_Require (dspAddr    != NULL) ;

    if (    (!(IS_VALID_PROCID (procId)))
        ||  (objCtx     == NULL)
        ||  (symbolName == NULL)
        ||  (dspAddr    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *dspAddr = (Uint32) NULL ;
        for (i = 0 ; (i < MAX_SYMBOLS) && (DSP_SUCCEEDED (status)) ; i++) {
            status = GEN_Strcmp (NOLOADER_symArray [i].symName,
                                 symbolName,
                                 &cmpResult) ;
            if (DSP_SUCCEEDED (status)) {
                if (cmpResult == 0) {
                    *dspAddr = NOLOADER_symArray [i].symValue ;
                }
            }

        }

        if ((*dspAddr) == ((Uint32) NULL)) {
            /* Symbol lookup not supported for any other symbols. */
            status = DSP_ENOTSUPPORTED ;
        }
    }

    TRC_1LEAVE ("NOLOADER_getSymbolAddress", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
