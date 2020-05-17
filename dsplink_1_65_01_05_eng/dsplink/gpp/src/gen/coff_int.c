/** ============================================================================
 *  @file   coff_int.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines generic functions of COFF loader.
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

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>

/*  ----------------------------------- Generic Functions           */
#include <coff_int.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID         ID_GEN_COFF

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define  SET_FAILURE_REASON TRC_SetReason (status, FID_C_GEN_COFF_INT, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   COFF_read8
 *
 *  @desc   Reads a Int8 from file.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Int8
COFF_read8 (IN KFileObject * fileObj)
{
    Int8 retVal = 0 ;

    TRC_1ENTER ("COFF_read8", fileObj) ;

    DBC_Require (fileObj != NULL) ;

    KFILE_Read ((Char8 *) &retVal, READ_REC_SIZE, sizeof (Int8), fileObj) ;

    TRC_0LEAVE ("COFF_read8") ;

    return retVal ;
}


/** ============================================================================
 *  @func   COFF_read16
 *
 *  @desc   Reads a Int16 from file.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Int16
COFF_read16 (IN KFileObject * fileObj, IN Bool swap)
{
    Int16 retVal ;

    TRC_2ENTER ("COFF_read16", fileObj, swap) ;

    DBC_Require (fileObj != NULL) ;

    KFILE_Read ((Char8 *) ((Pvoid) &retVal),
                READ_REC_SIZE,
                sizeof (Int16),
                fileObj) ;

    if (swap == TRUE) {
        retVal = BYTESWAP_WORD (retVal) ;
    }

    TRC_0LEAVE ("COFF_read16") ;

    return retVal ;
}


/** ============================================================================
 *  @func   COFF_read32
 *
 *  @desc   Reads a Int32 from file.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Int32
COFF_read32 (IN KFileObject * fileObj, IN Bool swap)
{
    Int32 retVal ;

    TRC_2ENTER ("COFF_read32", fileObj, swap) ;

    DBC_Require (fileObj != NULL) ;

    KFILE_Read ((Char8 *) ((Pvoid) &retVal),
                READ_REC_SIZE,
                sizeof (Int32),
                fileObj) ;

    if (swap == TRUE) {
        retVal = BYTESWAP_LONG (retVal) ;
    }

    TRC_0LEAVE ("COFF_read32") ;

    return retVal ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
