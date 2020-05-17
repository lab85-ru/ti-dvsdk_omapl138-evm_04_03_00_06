/** ============================================================================
 *  @file   procdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines data types and structures used by PROC component.
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


#if !defined (PROCDEFS_H)
#define PROCDEFS_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <gpptypes.h>
#include <linkcfgdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  IS_VALID_PROCID
 *
 *  @desc   Is the proc ID valid.
 *  ============================================================================
 */
#define IS_VALID_PROCID(id)   (id < MAX_DSPS)


/** ============================================================================
 *  @name   PROC_State
 *
 *  @desc   Enumerations to indicate processor states.
 *
 *  @field  ProcState_Unknown
 *              Unknown (possibly error) processor state.
 *  @field  ProcState_Setup
 *              Indicates that the PROC component has been setup.
 *  @field  ProcState_Loaded
 *              Indicates the DSP is loaded.
 *  @field  ProcState_Started
 *              Indicates the DSP is started.
 *  @field  ProcState_Stopped
 *              Indicates the DSP is stopped.
 *  @field  ProcState_Idle
 *              Indicates the DSP is idle.
 *  @field  ProcState_Reset
 *              Indicates the DSP is reset.
 *  @field  ProcState_Unavailable
 *              Indicates the DSP is unavailable to the physical transport.
 *  ============================================================================
 */
typedef enum {
    ProcState_Unknown     = 0u,
    ProcState_Setup       = 1u,
    ProcState_Loaded      = 2u,
    ProcState_Started     = 3u,
    ProcState_Stopped     = 4u,
    ProcState_Idle        = 5u,
    ProcState_Reset       = 6u,
    ProcState_Unavailable = 7u
} PROC_State ;


/** ============================================================================
 *  @name   PROC_Attrs
 *
 *  @desc   A placeholder for processor information.
 *
 *  @field  timeout
 *              Time out associated with a DSP.
 *  @field  dspCfgPtr
 *              Pointer to DSP side Configuration.
 *  ============================================================================
 */
typedef struct PROC_Attrs_tag {
    Uint32              timeout ;
    LINKCFG_DspConfig * dspCfgPtr ;
} PROC_Attrs ;


/*  ============================================================================
 *  @const  PROC_CTRL_CMD_GEN_BASE, PROC_CTRL_CMD_GEN_END
 *
 *  @desc   Range for PROC_Control () commands to be handled by the generic
 *          implementation of PROC component.
 *  ============================================================================
 */
#define PROC_CTRL_CMD_GEN_BASE                                     0x00000100u
#define PROC_CTRL_CMD_GEN_LAST                                     0x00000200u

/*  ============================================================================
 *  @const  PROC_CTRL_CMD_MAP_DSPMEM, PROC_CTRL_CMD_UNMAP_DSPMEM
 *          PROC_CTRL_CMD_MMU_ADD_ENTRY, PROC_CTRL_CMD_MMU_DEL_ENTRY
 *
 *  @desc   Commands for mapping/ unmapping DSP memory.
 *          Commands for add/ delete mmu entry.
 *  ============================================================================
 */
#define PROC_CTRL_CMD_MAP_DSPMEM          (PROC_CTRL_CMD_GEN_BASE + 0x00000001u)
#define PROC_CTRL_CMD_UNMAP_DSPMEM        (PROC_CTRL_CMD_GEN_BASE + 0x00000002u)
#define PROC_CTRL_CMD_MMU_ADD_ENTRY       (PROC_CTRL_CMD_GEN_BASE + 0x00000003u)
#define PROC_CTRL_CMD_MMU_DEL_ENTRY       (PROC_CTRL_CMD_GEN_BASE + 0x00000004u)

/** ============================================================================
 *  @name   ProcMemMapInfo
 *
 *  @desc   Control information for mapping DSP memory region in GPP's address
 *          space.
 *
 *  @field  dspAddr
 *              Address in the DSP memory space to be mapped
 *  @field  size
 *              Size of memory to be mapped
 *  @field  mappedAddr
 *              Mapped address in the user memory space
 *  @field  mappedSize
 *              Size of memory actually mapped
 *  ============================================================================
 */
typedef struct ProcMemMapInfo_tag {
    Uint32   dspAddr ;
    Uint32   size    ;
    Uint32   mappedAddr ;
    Uint32   mappedSize ;
} ProcMemMapInfo ;


/** ============================================================================
 *  @deprecated The deprecated data structure ProcState has been replaced
 *              with PROC_State.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define ProcState PROC_State

/** ============================================================================
 *  @deprecated The deprecated data structures ProcAttr has been replaced
 *              with PROC_Attrs.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define ProcAttr  PROC_Attrs


#if defined (__cplusplus)
}
#endif


#endif /* !defined (PROCDEFS_H) */
