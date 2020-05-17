/** ============================================================================
 *  @file   coff_64x.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines C64x specific functions of COFF loader.
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
#include <loaderdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>
#include <print.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

#include <coff_int.h>
#include <coff.h>
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
#define  COMPONENT_ID         ID_GEN_COFF_64x


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_GEN_COFF_64x, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @macro  ARG_PTR_SIZE
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  ARG_PTR_SIZE 4u


/** ============================================================================
 *  @func   COFF_isValidFile_64x
 *
 *  @desc   Checks to indicate if the file data format is valid for the given
 *          architecture.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_isValidFile_64x (IN  KFileObject * fileObj,
                      OUT Bool *        isValid)
{
    DSP_STATUS status     = DSP_SOK ;
    Bool       isValidId  = FALSE   ;
    Bool       isValidVer = FALSE   ;
    Int16      swapVal              ;
    Uint16     version              ;

    TRC_2ENTER ("COFF_isValidFile_64x", fileObj, isValid) ;

    DBC_Require (fileObj != NULL) ;
    DBC_Require (isValid != NULL) ;

    *isValid = FALSE ;
    status = KFILE_Seek (fileObj, 0, KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        version = (Uint16) COFF_read16 (fileObj, FALSE) ;

        if (version != COFF_VERSION) {
            if (BYTESWAP_WORD (version) != COFF_VERSION) {
                status = DSP_EFILE ;
                SET_FAILURE_REASON ;
            }
            else {
                isValidVer = TRUE ;
            }
        }
        else {
            isValidVer = TRUE ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        status = KFILE_Seek (fileObj, SWAP_LOCATION, KFILE_SeekSet) ;
        if (DSP_SUCCEEDED (status)) {
            swapVal = COFF_read16 (fileObj, FALSE) ;

            if (swapVal != COFF_MAGIC_64x) {
                if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_64x) {
                    status = DSP_EFILE ;
                    SET_FAILURE_REASON ;
                }
                else {
                    isValidId = TRUE ;
                }
            }
            else {
                isValidId = TRUE ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        *isValid = ((TRUE == isValidId) && (TRUE == isValidVer)) ;
    }

    TRC_1LEAVE ("COFF_isValidFile_64x", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_isSwapped_64x
 *
 *  @desc   Check to indicate if the file data is swapped.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_isSwapped_64x (IN  KFileObject * fileObj,
                    OUT Bool *        isSwapped)
{
    DSP_STATUS status  = DSP_SOK ;
    Int16      swapVal           ;

    TRC_2ENTER ("COFF_isSwapped_64x", fileObj, isSwapped) ;

    DBC_Require (fileObj != NULL)   ;
    DBC_Require (isSwapped != NULL) ;

    *isSwapped = FALSE ;
    status = KFILE_Seek (fileObj, SWAP_LOCATION, KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        swapVal = COFF_read16 (fileObj, FALSE) ;

        if (swapVal != COFF_MAGIC_64x) {
            if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_64x) {
                status = DSP_EFILE ;
                SET_FAILURE_REASON ;
            }
            else {
                *isSwapped = TRUE ;
            }
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        TRC_1PRINT (TRC_LEVEL3, "Swapped: %d\n", *isSwapped) ;
    }

    TRC_1LEAVE ("COFF_isSwapped_64x", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_fillArgsBuffer_64x
 *
 *  @desc   Allocates and fills up a buffer with arguments to be sent to DSP's
 *          "main" function for the 64x architecture.
 *
 *          The .args buffer for Dsp/Bios 5.xx has the form:
 *          argc,
 *      +-- argv,
 *      |   envp
 *      +-->argv[0], -------+
 *          argv[1],        |
 *          ...,            |
 *          argv[n],        |
 *          argv 0 string,<-+
 *          argv 1 string,
 *          ...
 *          argv n string,
 *
 *          The envp pointer needs to be populated for the arguments to be
 *          passed correctly to the DSP executable's 'main'.
 *          However, currently this is not required.
 *
 *          The .args buffer for Dsp/Bios 6.xx has the form:
 *          argc,
 *          argv[0], -------+
 *          argv[1],        |
 *          ...,            |
 *          argv[n],        |
 *          argv 0 string,<-+
 *          argv 1 string,
 *          ...
 *          argv n string,
 *
 *          New arguments do not support the environment pointer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_fillArgsBuffer_64x (IN  DspArch     dspArch,
                         IN  Uint32      argc,
                         IN  Char8 **    argv,
                         IN  Uint32      sectSize,
                         IN  Uint32      loadAddr,
                         IN  Uint32      wordSize,
                         IN  Endianism   endian,
                         IN  Void *      argsBuf)
{
    DSP_STATUS status    = DSP_SOK ;
    Int32 *    bufPtr    = NULL    ;
    Char8 *    srcPtr    = NULL    ;
    Int32 *    argvPtr   = NULL    ;
    Char8 *    dstPtr    = NULL    ;
    Int32 *    envPtr    = NULL    ;
    Char8      envc      = 1       ;
    Uint32     totalReqd           ;
    Uint32     length              ;
    Uint32     i                   ;

    TRC_6ENTER ("COFF_fillArgsBuffer_64x",
                argc,
                argv,
                sectSize,
                loadAddr,
                wordSize,
                argsBuf) ;

    DBC_Require (argc > 0) ;
    DBC_Require (argv != NULL) ;
    DBC_Require (argsBuf != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Compute total target buffer size required (in bytes). Memory requirement
     *  is calculated as:
     *      3 units for argc, argv and envp.
     *      argc units for storing the argv pointers.
     *      envc units for storing the envp pointers.
     *  For the current implementation 'envc' is 1 as the environment
     *  variables are not being passed, so it is set as NULL.
     *  ------------------------------------------------------------------------
     */
    if (dspArch == DspArch_C64x_Bios5) {
        totalReqd = (Uint32) ((3 + argc + envc) * ARG_PTR_SIZE) ;
    }
    else {
        /* For DspArch_C64x_Bios6 */
        totalReqd = (Uint32) ((1 + argc ) * ARG_PTR_SIZE) ;
    }

    for (i = 0 ; DSP_SUCCEEDED (status) && (i < argc) ; i++) {
        DBC_Assert (argv [i] != NULL) ;

        status = GEN_Strlen (argv [i], &length) ;
        if (DSP_SUCCEEDED (status)) {
            totalReqd += (length + 1) ;
            /* Extend to the next wordsize */
            totalReqd =  ((totalReqd + (ARG_PTR_SIZE - 1)) / ARG_PTR_SIZE)
                       * ARG_PTR_SIZE ;
        }
    }

    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        if (totalReqd > sectSize) {
            status = DSP_ESIZE ;
            SET_FAILURE_REASON ;
        }
        else {
            bufPtr = (Int32 *) argsBuf ;

            /*  ------------------------------------------------------------
             *  Initialize the argument buffer with 0.
             *  ------------------------------------------------------------
             */
            srcPtr = (Char8 *) argsBuf ;
            for (i = 0 ; i < sectSize ; i++) {
                *srcPtr++ = 0 ;
            }

            /*  ------------------------------------------------------------
             *  Fill the buffer according to the required format.
             *  ------------------------------------------------------------
             */
            *bufPtr++ = (Int32) argc ;
            if (dspArch == DspArch_C64x_Bios5) {
                    *bufPtr++ = (Int32)
                                  (loadAddr + (3 * ARG_PTR_SIZE)) ; /* argv */
                    *bufPtr++ = (Int32)
                                  (loadAddr + ((3 + argc + 1) * ARG_PTR_SIZE)) ;
            }

            argvPtr = bufPtr ; /* Pointer to argv[0] */
            bufPtr  += argc ;  /* Space for argv pointers */

            if (dspArch == DspArch_C64x_Bios5) {
                    envPtr  = bufPtr ; /* Pointer to first envp */
                    DBC_Assert ( envPtr != NULL) ;

                    bufPtr  += envc ;  /* Space for envp pointers */
                    *envPtr = (Int32) 0 ; /* No envp */
            }

            for ( ; argc > 0 ; argc--) {
                *argvPtr++ = (Int32) (  (  ((Char8 *) ((Pvoid) bufPtr))
                                         - ((Char8 *) ((Pvoid) argsBuf)))
                                      + loadAddr) ;

                srcPtr = *argv++ ;
                dstPtr = (Char8 *) ((Pvoid) bufPtr) ;
                while ((*srcPtr) != '\0') {
                    *dstPtr = (Char8)(*srcPtr) ;
                    dstPtr++ ;
                    srcPtr++ ;
                }
                *dstPtr++ = (Char8) '\0' ;
                bufPtr    = (Int32 *) ((Pvoid) dstPtr) ;
            }
        }
    }

    TRC_1LEAVE ("COFF_fillArgsBuffer_64x", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
