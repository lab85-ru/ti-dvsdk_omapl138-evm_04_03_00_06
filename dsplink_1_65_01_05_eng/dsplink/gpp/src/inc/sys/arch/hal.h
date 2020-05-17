/** ============================================================================
 *  @file   hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/
 *
 *  @desc   Defines interface exposed by HAL sub-component.
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


#if !defined (HAL_H)
#define HAL_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <_dspdefs.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* ============================================================================
 *  @func   FnPhyInit
 *
 *  @desc   Signature of function that initializes PCI device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPhyInit) (IN Void * halObject) ;


/* ============================================================================
 *  @func   FnPhyExit
 *
 *  @desc   Signature of function that finalizes PCI device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPhyExit) (IN Void * halObject) ;


/* ============================================================================
 *  @func   FnBootCtrl
 *
 *  @desc   Signature of function that is Boot controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  Boot Command.
 *  @arg    arg.
 *                  Command specific argument (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnBootCtrl) (IN         Void *          halObject,
                                  IN         DSP_BootCtrlCmd cmd,
                                  IN OUT     Pvoid           arg) ;


/* ============================================================================
 *  @func   FnIntCtrl
 *
 *  @desc   Signature of function that is Interrupt Controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  Boot Command.
 *  @arg    intId
 *                  Interrupt Identifier (Optional)
 *  @arg    arg.
 *                  Command specific argument (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnIntCtrl) (IN         Void *         halObject,
                                 IN         DSP_IntCtrlCmd cmd,
                                 IN         Uint32         intId,
                                 IN OUT     Pvoid          arg) ;


/* ============================================================================
 *  @func   FnMmuCtrl
 *
 *  @desc   Signature of function that is Interrupt Controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  Boot Command.
 *  @arg    memTable
 *                  Memory Table.
 *  @arg    intId
 *                  Interrupt Identifier (Optional)
 *  @arg    arg.
 *                  Command specific argument (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMmuCtrl) (IN         Void *         halObject,
                                 IN         DSP_MmuCtrlCmd      cmd,
                                 IN         LINKCFG_MemEntry *  memTable,
                                 IN         Uint32              memEntries,
                                 IN         ProcMemMapInfo *    mmuInfo) ;


/* ============================================================================
 *  @func   FnMapCtrl
 *
 *  @desc   Signature of function that is Mapping controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  MAP Command.
 *  @arg    dspBufAddr.
 *                  DSP address to be mapped.
 *  @arg    size.
 *                  Size of area to be mapped.
 *  @arg    bufVirtPtr.
 *                  virtual address of mapped area.
 *  @arg    bufPhysAddr.
 *                  Physical address of mapped area in HOST address space.
 *  @arg    mappedSize.
 *              Size of memory region mapped.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ERANGE
 *              A parameter is specified outside its valid range.
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMapCtrl) (IN      Void *          halObject,
                                 IN      DSP_MapCtrlCmd  cmd,
                                 IN      Uint32          dspBufAddr,
                                 IN      Uint32          size,
                                 OUT     Uint32 *        bufVirtPtr,
                                 OUT     Uint32 *        bufPhysAddr,
                                 OUT     Uint32 *        mappedSize) ;


/* ============================================================================
 *  @func   FnPwrCtrl
 *
 *  @desc   Signature of function that is Power Controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  Boot Command.
 *  @arg    arg.
 *                  Command specific argument (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPwrCtrl) (IN         Void *         halObject,
                                 IN         DSP_PwrCtrlCmd cmd,
                                 IN OUT     Pvoid          arg) ;


/* ============================================================================
 *  @func   FnRead
 *
 *  @desc   Signature of function that Reads a buffer from dsp memory (for now
 *          this is done as bytes).
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    dspAddr.
 *                  DSP Address to be used as read address.
 *  @arg    cBytes.
 *                  Number of bytes to transfer.
 *  @arg    readBuffer.
 *                  Destination buffer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnRead) (IN  Void *  halObject,
                              IN  Uint32  dspAddr,
                              IN  Uint32  cBytes,
                              OUT Char8 * readBuffer) ;


/* ============================================================================
 *  @func   FnWrite
 *
 *  @desc   Signature of function that Write a buffer to dsp memory (for now
 *          this is done as bytes).
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    dspAddr.
 *                  DSP Address to be used as read address.
 *  @arg    cBytes.
 *                  Number of bytes to transfer.
 *  @arg    writeBuffer.
 *                  Source buffer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnWrite) (IN  Void *  halObject,
                               IN  Uint32  dspAddr,
                               IN  Uint32  cBytes,
                               IN  Char8 * writeBuffer) ;


/* ============================================================================
 *  @func   FnReadDMA
 *
 *  @desc   Signature of function that DMAs contents from DSP memory to GPP
 *          Memory. Here read means DSP write.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    srcAddr.
 *                  Source Address.
 *  @arg    dstAddr.
 *                  Destination Address.
 *  @arg    size.
 *                  Number of bytes to transfer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Not able to do read DMA for specified size.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnReadDMA) (IN Void *  halObject,
                                 IN Uint32  srcAddr,
                                 IN Uint32  dstAddr,
                                 IN Uint32  size) ;


/* ============================================================================
 *  @func   FnWriteDMA
 *
 *  @desc   Signature of function that DMAs contents from GPP memory to DSP
 *          Memory. Here write means DSP read.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    srcAddr.
 *                  Source Address.
 *  @arg    dstAddr.
 *                  Destination Address.
 *  @arg    size.
 *                  Number of bytes to transfer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Not able to do write DMA for specified size.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
typedef DSP_STATUS (*FnWriteDMA) (IN Void   *  halObject,
                                  IN Uint32    srcAddr,
                                  IN Uint32    dstAddr,
                                  IN Uint32    size) ;


/** ============================================================================
 *  @name   HAL_Interface
 *
 *  @desc   Interface functions exported by the DSP subcomponent.
 *
 *  @field  phyInit
 *              Function pointer to Initializes physical interface function
 *              for the DSP.
 *  @field  phyExit
 *              Function pointer to Finalizes physical interface function
 *              for the DSP.
 *  @field  bootCtrl
 *              Function pointer to boot control function for the DSP.
 *  @field  intCtrl
 *              Function pointer to intterrupt control function for the DSP.
 *  @field  mapCtrl
 *              Function pointer to map control function for the DSP.
 *  @field  pwrCtrl
 *              Function pointer to power control function for the DSP.
 *  @field  read
 *              Function pointer to read memory function for the DSP.
 *  @field  write
 *              Function pointer to write memory function for the DSP.
 *  @field  readDMA
 *              Function pointer to read DMA function for the DSP.
 *  @field  writeDMA
 *              Function pointer to write DMA function for the DSP.
 *  ============================================================================
 */
typedef struct HAL_Interface_tag {
    FnPhyInit  phyInit  ;
    FnPhyExit  phyExit  ;
    FnBootCtrl bootCtrl ;
    FnIntCtrl  intCtrl  ;
    FnMmuCtrl  mmuCtrl ;
    FnMapCtrl  mapCtrl  ;
    FnPwrCtrl  pwrCtrl  ;
    FnRead     read     ;
    FnWrite    write    ;
    FnReadDMA  readDMA  ;
    FnWriteDMA writeDMA ;
} HAL_Interface ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (HAL_H) */
