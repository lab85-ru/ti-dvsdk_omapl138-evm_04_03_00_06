/** ============================================================================
 *  @file   coff_55x.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines C55x specific functions of COFF loader.
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
#include <coff_55x.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID         ID_GEN_COFF_55x

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_GEN_COFF_55x, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @macro  LARGE_MEM_MODEL_SIZE
 *
 *  @desc   Size for the large memory model
 *  ============================================================================
 */
#define  LARGE_MEM_MODEL_SIZE           4u


/** ============================================================================
 *  @func   COFF_isValidFile_55x
 *
 *  @desc   Checks to indicate if the file data format is valid for 55x
 *          architecture.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_isValidFile_55x (IN  KFileObject * fileObj,
                      OUT Bool *        isValid)
{
    DSP_STATUS status     = DSP_SOK ;
    Bool       isValidId  = FALSE   ;
    Bool       isValidVer = FALSE   ;
    Int16      swapVal              ;
    Uint16     version              ;

    TRC_2ENTER ("COFF_isValidFile_55x", fileObj, isValid) ;

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

            if (swapVal != COFF_MAGIC_55x) {
                if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_55x) {
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

    TRC_1LEAVE ("COFF_isValidFile_55x", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_isSwapped_55x
 *
 *  @desc   Checks to indicate if the file data is swapped for 55x architecture.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_isSwapped_55x (IN  KFileObject * fileObj,
                    OUT Bool *        isSwapped)
{
    DSP_STATUS status  = DSP_SOK ;
    Int16      swapVal           ;

    TRC_2ENTER ("COFF_isSwapped_55x", fileObj, isSwapped) ;

    DBC_Require (fileObj != NULL)   ;
    DBC_Require (isSwapped != NULL) ;

    *isSwapped = FALSE ;
    status = KFILE_Seek (fileObj, SWAP_LOCATION, KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        swapVal = COFF_read16 (fileObj, FALSE) ;

        if (swapVal != COFF_MAGIC_55x) {
            if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_55x) {
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

    TRC_1LEAVE ("COFF_isSwapped_55x", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_fillArgsBuffer_55x
 *
 *  @desc   Allocates and fills up a buffer with arguments to be sent to DSP's
 *          "main" function for the 55x architecture.
 *
 *          The .args buffer has the form:
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
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_fillArgsBuffer_55x (IN  Uint32      argc,
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
    Int16 *    dstPtr    = NULL    ;
    Int32 *    argvPtr   = NULL    ;
    Int32 *    envPtr    = NULL    ;
    Int16      envc      = 0       ;
    Uint32     totalReqd           ;
    Uint32     length              ;
    Uint32     i                   ;

    TRC_6ENTER ("COFF_fillArgsBuffer_55x",
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
     *      argc + 1 units for storing the argv pointers.
     *      envc + 1 units for storing the envp pointers.
     *  For the current implementation 'envc' is 0 as the environment
     *  variables are not being passed.
     *  ------------------------------------------------------------------------
     */
    totalReqd = (Uint32) ((3 + argc + 1 + envc + 1) * LARGE_MEM_MODEL_SIZE) ;
    for (i = 0 ; DSP_SUCCEEDED (status) && (i < argc) ; i++) {

        DBC_Assert (argv [i] != NULL) ;

        status = GEN_Strlen (argv [i], &length) ;
        if (DSP_SUCCEEDED (status)) {
            totalReqd += ((length + 1) * wordSize) ;
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
            loadAddr /= wordSize ;

            *((Int16 *) ((Pvoid) bufPtr)) = BYTESWAP_WORD (argc) ;
            bufPtr++;

            *bufPtr++ = BYTESWAP_LONG (loadAddr + (3 * wordSize)) ;

            *bufPtr++ = BYTESWAP_LONG (loadAddr + ((3 + argc + 1) * wordSize)) ;

            argvPtr   = bufPtr ;
            bufPtr   += argc + 1 ;

            envPtr    = bufPtr ;

            DBC_Assert ( envPtr != NULL) ;
            DBC_Assert ( argvPtr != NULL) ;

            bufPtr   += envc + 1 ;

            for ( ; argc > 0 ; argc--) {
                *argvPtr++ =
                        BYTESWAP_LONG (  (  (Uint32) (bufPtr) - (Uint32) argsBuf
                                          / wordSize)
                                       + loadAddr) ;

                srcPtr = *argv++ ;
                dstPtr = (Int16 *) ((Pvoid) bufPtr) ;
                while ((*srcPtr) != '\0') {
                    Uint16  temp = *srcPtr & 0xFFFF ;
                    Uint16  temp1 = BYTESWAP_WORD (temp) ;
                    *dstPtr = (Int16) temp1 ;
                    dstPtr++ ;
                    srcPtr++ ;
                }
                *dstPtr++ = (Int16) 0 ;
                bufPtr    = (Int32 *) ((Pvoid) dstPtr) ;
            }
            *argvPtr = 0L ;
            *envPtr  = 0L ;
        }
    }

    TRC_1LEAVE ("COFF_fillArgsBuffer_55x", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
