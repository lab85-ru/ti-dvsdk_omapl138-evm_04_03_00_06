/** ============================================================================
 *  @file   _loaderdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines data types and structures used by the DSP/BIOS(tm) Link
 *          loader.
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


#if !defined (_LOADERDEFS_H)
#define _LOADERDEFS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#if defined (PROC_COMPONENT)
#include <procdefs.h>
#endif /* #if defined (PROC_COMPONENT) */
#include <kfiledefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   FnWriteDspMem
 *
 *  @desc   Signature of function that writes to DSP memory.
 *
 *  @arg    dspId
 *              DSP ID of DSP whoose memory is to be written.
 *  @arg    dspAddr
 *              Address to which data needs to be written.
 *  @arg    endianInfo
 *              This specifies endianness of the data.
 *  @arg    numBytes
 *              Number of bytes to be written.
 *  @arg    buffer
 *              Buffer in which to store the data to write.
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              DSP not in the right state to execute this function.
 *
 *  ============================================================================
 */
typedef DSP_STATUS (*FnWriteDspMem) (IN  ProcessorId  dspId,
                                     IN  Uint32       dspAddr,
                                     IN  Endianism    endianInfo,
                                     IN  Uint32       numBytes,
                                     IN  Uint8 *      buffer) ;

/** ============================================================================
 *  @func   FnAddrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    addr
 *              Address to be converted. If DSP address, the addr parameter
 *              reflects the DSP MADU address.
 *  @arg    type
 *              Type of address conversion.
 *
 *  @ret    Converted address.
 *              Operation successfully completed.
 *          ADDRMAP_INVALID.
 *              Specified address is not in mapped range.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    FnAddrConvert
 *  ============================================================================
 */
typedef Uint32 (*FnAddrConvert) (IN  ProcessorId        dspId,
                                 IN  Uint32             addr,
                                 IN  DSP_AddrConvType   type) ;


/** ============================================================================
 *  @name   LoaderInitArgs
 *
 *  @desc   Initialization arguments for the loader.
 *
 *  @field  dspArch
 *              Architecture of the dsp.
 *  @field  kfileIntf
 *              KFILE interface used by this loader.
 *  ============================================================================
 */
typedef struct LoaderInitArgs_tag {
    DspArch           dspArch     ;
    KFILE_Interface * kfileIntf ;
} LoaderInitArgs ;

/** ============================================================================
 *  @name   LoaderObject
 *
 *  @desc   This object is used to pass arguments to Loader component.
 *
 *  @field  objCtx
 *              Object context for the loader for the file to be loaded.
 *  @field  dspArch
 *              Architecture of the dsp.
 *  @field  endian
 *              Endianism of the processor.
 *  @field  maduSize
 *              Madu size on the dsp.
 *  @field  fnWriteDspMem
 *              Function pointer to write function for the DSP.
 *  @field  fnAddrConvert
 *              Function pointer to do address translation between GPP and DSP.
 *
 *  @see    None
 *  ============================================================================
 */
typedef struct LoaderObject_tag {
    Void *          objCtx        ;
    DspArch         dspArch       ;
    Endianism       endian        ;
    Uint32          maduSize      ;
    FnWriteDspMem   fnWriteDspMem ;
    FnAddrConvert   fnAddrConvert ;
} LoaderObject ;


/** ============================================================================
 *  @name   FnLoaderInit
 *
 *  @desc   The Function pointer providing the abstraction to the loader's
 *          init function.
 *
 *  @arg    procId
 *              Processor Identifier for the target DSP.
 *  @arg    baseImage
 *              Identifier for the file.
 *  @arg    args
 *              Arguments for initialization of the loader.
 *  @arg    objCtx
 *              Location to receive the context object for the loader.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              File not found.
 *          DSP_EMEMORY
 *              Out of memory error.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLoaderInit) (IN  ProcessorId      procId,
                                    IN  Pstr             baseImage,
                                    IN  LoaderInitArgs * args,
                                    OUT Pvoid *          objCtx) ;


/** ============================================================================
 *  @name   FnLoaderExit
 *
 *  @desc   The Function pointer providing the abstraction to the loader's
 *          exit function.
 *
 *  @arg    objCtx
 *              The context object obtained through COFF_init.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              File is not open.
 *          DSP_EPOINTER
 *              Invalid file object.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLoaderExit) (IN  Pvoid objCtx) ;


/** ============================================================================
 *  @name   FnLoad
 *
 *  @desc   The Function pointer providing the abstraction to the loader's
 *          load function.
 *
 *  @arg    dspId
 *              Target DSP identifier where the base image is to be loaded.
 *  @arg    loaderObj
 *              This object is used to receive arguments from PMGR.
 *  @arg    argc
 *              Number of arguments to be passed to the base image upon start.
 *  @arg    argv
 *              Arguments to be passed to DSP main application.
 *  @arg    entryPt
 *              Argument for returning entry address for the executable.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              Invalid base image
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP
 *          DSP_EFAIL
 *              General failure, unable to load image onto DSP
 *          DSP_EINVALIDARG
 *              Invalid dspId argument.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLoad) (IN  ProcessorId     dspId,
                              IN  LoaderObject *  loaderObj,
                              IN  Uint32          argc,
                              IN  Char8 **        argv,
                              OUT Uint32 *        entryPt) ;

/** ============================================================================
 *  @name   FnLoadSection
 *
 *  @desc   The Function pointer providing the abstraction to the loader's
 *          loadSection function.
 *
 *  @arg    dspId
 *              Target DSP identifier where the section is to be loaded.
 *  @arg    loaderObj
 *              This object is used to receive arguments from PMGR.
 *  @arg    sectId
 *              Identifier for section to load.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              Invalid base image
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP
 *          DSP_EFAIL
 *              General failure, unable to load image onto DSP
 *          DSP_EINVALIDARG
 *              Invalid dspId argument.
 *          DSP_EINVALIDSECT
 *              Invalid section name.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLoadSection) (IN  ProcessorId     dspId,
                                     IN  LoaderObject *  loaderObj,
                                     IN  Uint32          sectId) ;


/** ============================================================================
 *  @name   FnGetSymbolAddress
 *
 *  @desc   The Function pointer providing the abstraction to the loader's
 *          GetSymbolAddress function.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    objCtx
 *              Context of the loader object.
 *  @arg    symbolName
 *              Name of the symbol to be looked up.
 *  @arg    dspAddr
 *              Location to receive the DSP address for the specified symbol.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ENOTSUPPORTED
 *              Symbol lookup feature is not supported by the loader.
 *          DSP_ENOTFOUND
 *              Symbol not found.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnGetSymbolAddress) (IN   ProcessorId     procId,
                                          IN   Void *          objCtx,
                                          IN   Char8 *         symbolName,
                                          OUT  Uint32 *        dspAddr) ;


/** ============================================================================
 *  @name   LOADER_Interface
 *
 *  @desc   Structure containing interface functions exported by the loader
 *          subcomponent.
 *
 *  @field  init
 *              Function pointer providing the abstraction to the loader's
 *              init module.
 *  @field  exit
 *              Function pointer providing the abstraction to the loader's
 *              exit module.
 *  @field  load
 *              Function pointer providing the abstraction to the loader's
 *              load module.
 *  @field  loadSection
 *              Function pointer providing the abstraction to the loader's
 *              loadSection module.
 *  @field  getSymbolAddress
 *              Function pointer providing the abstraction to the loader's
 *              GetSymbolAddress module.
 *  ============================================================================
 */
typedef struct LOADER_Interface_tag {
    FnLoaderInit        init              ;
    FnLoaderExit        exit              ;
    FnLoad              load              ;
    FnLoadSection       loadSection       ;
    FnGetSymbolAddress  getSymbolAddress  ;
} LOADER_Interface ;


#if defined (__cplusplus)
}
#endif

#endif /* !define (_LOADERDEFS_H) */
