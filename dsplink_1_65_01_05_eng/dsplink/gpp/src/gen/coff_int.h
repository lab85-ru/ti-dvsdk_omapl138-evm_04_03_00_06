/** ============================================================================
 *  @file   coff_int.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines interface for generic functions and macros of COFF loader.
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


#if !defined (COFF_INT_H)
#define COFF_INT_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  READ_REC_SIZE
 *
 *  @desc   Record size to be specified when reading file thru' KFILE_Read
 *  ============================================================================
 */
#define READ_REC_SIZE   1u


/** ============================================================================
 *  @func   COFF_read8
 *
 *  @desc   Reads an Int8 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int8
COFF_read8 (IN KFileObject * fileObj) ;


/** ============================================================================
 *  @func   COFF_read16
 *
 *  @desc   Reads an Int16 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int16
COFF_read16 (IN KFileObject * fileObj, IN Bool swap) ;


/** ============================================================================
 *  @func   COFF_read32
 *
 *  @desc   Reads an Int32 from file.
 *
 *  @arg    fileObj
 *              File to read from.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *
 *  @ret    The read value.
 *
 *  @enter  fileObj must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int32
COFF_read32 (IN KFileObject * fileObj, IN Bool swap) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (COFF_INT_H) */
