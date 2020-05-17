/** ============================================================================
 *  @file   _dspdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines the DSP object structure and associated structures.
 *          Some structures, already defined in CFG, have been redefined here
 *          to make the DSP subcomponent independent of CFG.
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


#if !defined (_DSPDEFS_H)
#define _DSPDEFS_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <procdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*  ============================================================================
 *  @const  ADDRMAP_INVALID
 *
 *  @desc   Indicates DSP address mapping to GPP space is invalid.
 *  ============================================================================
 */
#define ADDRMAP_INVALID  0x0u


#if defined (DDSP_PROFILE)
/*  ============================================================================
 *  @name   DSP_Stats
 *
 *  @desc   Forward declaration of DSP Statistics Object.
 *  ============================================================================
 */
typedef struct DSP_Stats_tag DSP_Stats ;
#endif  /* defined (DDSP_PROFILE) */


/** ============================================================================
 *  @name   DSP_Interface_tag
 *
 *  @desc   Forward declaration of DSP Interface.
 *  ============================================================================
 */
typedef struct DSP_Interface_tag DSP_Interface ;


/*  ============================================================================
 *  @name   DSP_Object
 *
 *  @desc   This structure defines the DSP object, which contains all state
 *          information required by the DSP component.
 *
 *  @field  dspId
 *              DSP identifier.
 *  @field  halObject
 *              HAL object.
 *  @field  dspInterface
 *              Function table for the DSP APIs.
 *  @field  dspStats
 *              Profiling information related to the target DSP.
 *              Defined only when profiling is enabled.
 *  ============================================================================
 */
typedef struct DSP_Object_tag {
    Uint32          dspId        ;
    Pvoid           halObject    ;
    DSP_Interface * dspInterface    ;
#if defined (DDSP_PROFILE)
    DSP_Stats       dspStats     ;
#endif /* if defined (DDSP_PROFILE) */
} DSP_Object ;


/** ============================================================================
 *  @name   DSP_AddrConvType
 *
 *  @desc   Defines the types of address conversion performed by the
 *          DSP_addrConvert () API.
 *
 *  @field  GppToDsp
 *              GPP to DSP address conversion.
 *  @field  DspToGpp
 *              DSP to GPP address conversion.
 *  ============================================================================
 */
typedef enum {
    GppToDsp  = 0u,
    DspToGpp  = 1u
} DSP_AddrConvType ;

/** ============================================================================
 *  @name   DSP_IntCtrlCmd
 *
 *  @desc   Defines the types of interrupt control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_IntCtrlCmd_Enable
 *              Enable interrupt
 *  @field  DSP_IntCtrlCmd_Disable
 *              Disable interrupt
 *  @field  DSP_IntCtrlCmd_Send
 *              Send interrupt
 *  @field  DSP_IntCtrlCmd_Clear
 *              Clear interrupt
 *  @field  DSP_IntCtrlCmd_WaitClear
 *              Wait for interrupt to be cleared
 *  @field  DSP_IntCtrlCmd_Check
 *              Check whether DSP has generated INT or not.
 *  @field  DSP_IntCtrlCmd_GetIntId
 *              get the interrupt Id (used only on PCI)
 *  ============================================================================
 */
typedef enum {
    DSP_IntCtrlCmd_Enable    = 0u,
    DSP_IntCtrlCmd_Disable   = 1u,
    DSP_IntCtrlCmd_Send      = 2u,
    DSP_IntCtrlCmd_Clear     = 3u,
    DSP_IntCtrlCmd_WaitClear = 4u,
    DSP_IntCtrlCmd_Check     = 5u,
    DSP_IntCtrlCmd_GetIntId  = 6u
} DSP_IntCtrlCmd ;


/** ============================================================================
 *  @name   DSP_MmuCtrlCmd
 *
 *  @desc   Defines the types of MMU control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_MmuCtrlCmd_Enable
 *              Enable interrupt
 *  @field  DSP_MmuCtrlCmd_Disable
 *              Disable interrupt
 *  @field  DSP_MmuCtrlCmd_AddEntry
 *              Add MMU entry
 *  @field  DSP_MmuCtrlCmd_DeleteEntry
 *              Delete MMU entry
 *  ============================================================================
 */
typedef enum {
    DSP_MmuCtrlCmd_Enable    = 0u,
    DSP_MmuCtrlCmd_Disable   = 1u,
    DSP_MmuCtrlCmd_AddEntry     = 2u,
    DSP_MmuCtrlCmd_DeleteEntry  = 3u
} DSP_MmuCtrlCmd ;


/** ============================================================================
 *  @name   DSP_BootCtrlCmd
 *
 *  @desc   Defines the types of Boot control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_BootCtrlCmd_SetEntryPoint
 *              Sets entry point
 *  @field  DSP_BootCtrlCmd_SetBootComplete
 *              Indicate complete of boot sequence
 *  @field  DSP_BootCtrlCmd_ResetBootComplete
 *              Reset the boot complete boot flag.
 *  ============================================================================
 */
typedef enum {
    DSP_BootCtrlCmd_SetEntryPoint     = 0u,
    DSP_BootCtrlCmd_SetBootComplete   = 1u,
    DSP_BootCtrlCmd_ResetBootComplete = 2u
} DSP_BootCtrlCmd ;


/** ============================================================================
 *  @name   DSP_MapCtrlCmd
 *
 *  @desc   Defines the types of Map control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_MapCtrlCmd_Map
 *              Maps the given dsp address
 *  @field  DSP_MapCtrlCmd_Unmap
 *              Maps the given previous dsp address.
 *  @field  DSP_MapCtrlCmd_SetShared
 *              Maps the shared memory to the given dsp address.
 *  ============================================================================
 */
typedef enum {
    DSP_MapCtrlCmd_Map       = 0u,
    DSP_MapCtrlCmd_Unmap     = 1u,
    DSP_MapCtrlCmd_SetShared = 2u
} DSP_MapCtrlCmd ;


/** ============================================================================
 *  @name   DSP_PwrCtrlCmd
 *
 *  @desc   Defines the types of Power control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_PwrCtrlCmd_PowerUp
 *              Power the DSP device.
 *  @field  DSP_PwrCtrlCmd_PowerDown
 *              Power down the DSP device.
 *  @field  DSP_PwrCtrlCmd_Reset
 *              Reset the DSP device.
 *  @field  DSP_PwrCtrlCmd_Release
 *              Release the DSP device from reset.
 *  @field  DSP_PwrCtrlCmd_PeripheralUp
 *              Power the required peripherals.
 *  ============================================================================
 */
typedef enum {
    DSP_PwrCtrlCmd_PowerUp   = 0u,
    DSP_PwrCtrlCmd_PowerDown = 1u,
    DSP_PwrCtrlCmd_Reset     = 2u,
    DSP_PwrCtrlCmd_Release   = 3u,
    DSP_PwrCtrlCmd_PeripheralUp = 4u
} DSP_PwrCtrlCmd ;


/** ============================================================================
 *  @name   DSP_DmaCtrlCmd
 *
 *  @desc   Defines the types of Power control commands handled by the DSP
 *          component.
 *
 *  @field  DSP_DmaCtrlCmd_GppToDsp
 *              Start DMA from GPP to DSP.
 *  @field  DSP_DmaCtrlCmd_DspToGpp
 *              Start DMA from DSP to GPP.
 *  ============================================================================
 */
typedef enum {
    DSP_DmaCtrlCmd_GppToDsp = 0u,
    DSP_DmaCtrlCmd_DspToGpp = 1u
} DSP_DmaCtrlCmd ;


/** ============================================================================
 *  @name   DspDmaArgs
 *
 *  @desc   This structure defines the argument neccesary to call DSP DMA
 *          functions.
 *
 *  @field  dspId
 *              ID of the processor with which the memory manager is shared.
 *  @field  srcAddr
 *              Source address for DMA.
 *  @field  dstAddr
 *              Destination address for DMA.
 *  @field  size
 *              Length of the shared memory region.
 *  ============================================================================
 */
typedef struct DspDmaArgs_tag {
    ProcessorId     dspId   ;
    Uint32          srcAddr ;
    Uint32          dstAddr ;
    Uint32          size    ;
} DspDmaArgs ;


/** ============================================================================
 *  @name   FnDspInit
 *
 *  @desc   Signature of function that resets the DSP and inits the
 *          components required by DSP.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspInit) (IN ProcessorId  dspId,
                                 IN DSP_Object * dspState) ;

/** ============================================================================
 *  @name   FnDspExit
 *
 *  @desc   Signature of function that resets the DSP.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspExit) (IN ProcessorId  dspId,
                                 IN DSP_Object * dspState) ;

/** ============================================================================
 *  @name   FnDspStart
 *
 *  @desc   Signature of function that causes DSP to start execution from
 *          the given DSP address.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              Address to start execution from.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspStart) (IN ProcessorId  dspId,
                                  IN DSP_Object * dspState,
                                  IN Uint32       dspAddr) ;

/** ============================================================================
 *  @name   FnDspStop
 *
 *  @desc   Signature of function that stops execution on DSP.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspStop) (IN ProcessorId  dspId,
                                 IN DSP_Object * dspState) ;

/** ============================================================================
 *  @name   FnDspIdle
 *
 *  @desc   Signature of function that idles the DSP.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspIdle) (IN ProcessorId  dspId,
                                 IN DSP_Object * dspState) ;

/** ============================================================================
 *  @name   FnDspIntCtrl
 *
 *  @desc   Signature of function that perform the specified DSP interrupt
 *          control activity.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    intId
 *              Interrupt ID.
 *  @arg    cmd
 *              Interrupt control command to be performed.
 *  @arg    arg
 *              Optional input/output argument specific to each control command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspIntCtrl) (IN         ProcessorId       dspId,
                                    IN         DSP_Object *      dspState,
                                    IN         Uint32            intId,
                                    IN         DSP_IntCtrlCmd    cmd,
                                    IN OUT     Pvoid             arg) ;

/** ============================================================================
 *  @name   FnDspRead
 *
 *  @desc   Signature of function that reads data from DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              DSP address to read from.
 *  @arg    endianism
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to read.
 *  @arg    buffer
 *              Buffer to hold the read data.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspRead) (IN  ProcessorId  dspId,
                                 IN  DSP_Object * dspState,
                                 IN  Uint32       dspAddr,
                                 IN  Endianism    endianism,
                                 IN  Uint32       numBytes,
                                 OUT Uint8 *      buffer) ;

/** ============================================================================
 *  @name   FnDspWrite
 *
 *  @desc   Signature of function that writes data to DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              DSP address to write to.
 *  @arg    endianism
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to write.
 *  @arg    buffer
 *              Buffer containing data to be written.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              dspId is invalid.
 *          DSP_EPOINTER
 *              DSP_setup function wasn't called before calling this function.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspWrite) (IN ProcessorId  dspId,
                                  IN DSP_Object * dspState,
                                  IN Uint32       dspAddr,
                                  IN Endianism    endianism,
                                  IN Uint32       numBytes,
                                  IN Uint8 *      buffer) ;

/** ============================================================================
 *  @name   FnDspAddrConvert
 *
 *  @desc   Signature of function that converts address between GPP and DSP
 *          address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
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
 *  ============================================================================
 */
typedef Uint32 (*FnDspAddrConvert) (IN  ProcessorId        dspId,
                                    IN  DSP_Object *       dspState,
                                    IN  Uint32             addr,
                                    IN  DSP_AddrConvType   type) ;

/** ============================================================================
 *  @name   FnDspControl
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    cmd
 *              Command id.
 *  @arg    arg
 *              Optional argument for the specified command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments specified.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDspControl) (IN  ProcessorId dspId,
                                    IN DSP_Object * dspState,
                                    IN  Int32       cmd,
                                        Pvoid       arg) ;


/** ============================================================================
 *  @name   DSP_Interface_tag
 *
 *  @desc   Interface functions exported by the DSP subcomponent.
 *
 *  @field  init
 *              Function pointer to init function for the DSP.
 *  @field  exit
 *              Function pointer to exit function for the DSP.
 *  @field  start
 *              Function pointer to start function for the DSP.
 *  @field  stop
 *              Function pointer to stop function for the DSP.
 *  @field  idle
 *              Function pointer to idle function for the DSP.
 *  @field  intCtrl
 *              Function pointer to interrupt control function for the DSP.
 *  @field  read
 *              Function pointer to read function for the DSP.
 *  @field  write
 *              Function pointer to write function for the DSP.
 *  @field  addrConvert
 *              Function pointer to address conversion function for the DSP.
 *  @field  control
 *              Function pointer to device dependent control functionionality
 *              for the DSP.
 *  @field  debug
 *              Function pointer to debug function for the DSP.
 *  ============================================================================
 */
struct DSP_Interface_tag {
    FnDspInit               init             ;
    FnDspExit               exit             ;
    FnDspStart              start            ;
    FnDspStop               stop             ;
    FnDspIdle               idle             ;
    FnDspIntCtrl            intCtrl          ;
    FnDspRead               read             ;
    FnDspWrite              write            ;
    FnDspAddrConvert        addrConvert      ;
    FnDspControl            control          ;
} ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (_DSPDEFS_H) */
