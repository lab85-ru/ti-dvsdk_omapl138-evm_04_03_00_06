/** ============================================================================
 *  @file   loaderdefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines data types and structures used by the DSP/BIOS(tm) Link
 *          loader.
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


#if !defined (LOADERDEFS_H)
#define LOADERDEFS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <procdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   BINLOADER_ImageInfo
 *
 *  @desc   Structure defining information about the image to be loaded. A
 *          pointer to this structure is passed during the PROC_load () function
 *          as the imagePath, when the binary loader is used.
 *
 *  @field  gppAddr
 *              Address of the file to be loaded, in GPP address space.
 *  @field  dspLoadAddr
 *              DSP address where the binary file is to be loaded.
 *  @field  dspRunAddr
 *              DSP address from where the binary file execution is to be
 *              started.
 *  @field  fileSize
 *              Size of the file to be loaded in bytes.
 *  @field  shmBaseAddr
 *              Address of the symbol where the DSPLINK shared memory base
 *              address is stored.
 *
 *  @see    PROC_load ()
 *  ============================================================================
 */
typedef struct BINLOADER_ImageInfo_tag {
    Uint32  gppAddr     ;
    Uint32  dspLoadAddr ;
    Uint32  dspRunAddr  ;
    Uint32  fileSize    ;
    Uint32  shmBaseAddr ;
} BINLOADER_ImageInfo ;


/** ============================================================================
 *  @name   STATICLOADER_ImageInfo
 *
 *  @desc   Structure defining information about the image to be loaded. A
 *          pointer to this structure is passed during the PROC_load () function
 *          as the imagePath, when the static loader is used.
 *
 *  @field  dspRunAddr
 *              DSP address from where the binary file execution is to be
 *              started.
 *  @field  argsAddr
 *              Address of the .args section.
 *  @field  argsSize
 *              Size of the .args section.
 *  @field  shmBaseAddr
 *              Address of the symbol where the DSPLINK shared memory base
 *              address is stored.
 *
 *  @see    PROC_load ()
 *  ============================================================================
 */
typedef struct STATICLOADER_ImageInfo_tag {
    Uint32  dspRunAddr  ;
    Uint32  argsAddr    ;
    Uint32  argsSize    ;
    Uint32  shmBaseAddr ;
} STATICLOADER_ImageInfo ;


/** ============================================================================
 *  @name   NOLOADER_ImageInfo
 *
 *  @desc   Structure defining information about the image to be loaded. A
 *          pointer to this structure is passed during the PROC_load () function
 *          as the imagePath, when the dummy loader is used.
 *
 *  @field  dspRunAddr
 *              DSP address from where the binary file execution is to be
 *              started.
 *  @field  argsAddr
 *              Address of the .args section.
 *  @field  argsSize
 *              Size of the .args section.
 *  @field  shmBaseAddr
 *              Address of the symbol where the DSPLINK shared memory base
 *              address is stored.
 *
 *  @see    PROC_load ()
 *  ============================================================================
 */
typedef struct NOLOADER_ImageInfo_tag {
    Uint32  dspRunAddr  ;
    Uint32  argsAddr    ;
    Uint32  argsSize    ;
    Uint32  shmBaseAddr ;
} NOLOADER_ImageInfo ;


/** ============================================================================
 *  @name   COFFLOADER_ImageInfo
 *
 *  @desc   Structure defining information about the image to be loaded. A
 *          pointer to this structure is passed during the PROC_load () function
 *          as the imagePath, when the COFF loader to be read from memory is
 *          used.
 *
 *  @field  fileAddr
 *              GPP kernel address from where the COFF file is to be read.
 *  @field  size
 *              Size of .out file.
 *
 *  @see    PROC_load ()
 *  ============================================================================
 */
typedef struct COFFLOADER_ImageInfo_tag {
    Uint32  fileAddr  ;
    Uint32  size ;
} COFFLOADER_ImageInfo ;


/** ============================================================================
 *  @deprecated The deprecated data structure BinLoaderImageInfo has been
 *              replaced with BINLOADER_ImageInfo.
 *
 *  ============================================================================
 */
#define BinLoaderImageInfo BINLOADER_ImageInfo


#if defined (__cplusplus)
}
#endif

#endif /* !define (LOADERDEFS_H) */
