/*
 * DSPLINK_CFG_update.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/** ============================================================================
 *  @file   CFG_OMAPL138GEM_SHMEM.c
 *
 *  @path   $(DSPLINK)/config/all/
 *
 *  @desc   Defines the configuration information for DSP/BIOS LINK for the
 *          OMAPL138 platform.
 *
 *  @ver    1.65.00.02
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

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <procdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/** ============================================================================
 *                                DSPLINK MEMORY SECTION DIAGRAM
 *
 *         |--------------[ 0xXXXXXX00 ]---------------------------------|
 *         |                                                             |
 *         |                   RESET VECTOR (0x80)                       |
 *         |               Last two nibbles must be zero                 |
 *         |                                                             |
 *         |-------------------------------------------------------------|
 *         |                                                             |
 *         |                   DSP CODE/DATA                             |
 *         |               DSP executable is loaded to this section      |
 *         |                                                             |
 *         |=============================================================|
 *         |                                                             |
 *         |                   DSPLINK SHARED MEMORY0                    |
 *         |               Shared control structure is loaded here       |
 *         |                                                             |
 *         |-------------------------------------------------------------|
 *         |                                                             |
 *         |                   DSPLINK SHARED MEMORY1                    |
 *         |               Shared control structure is loaded here       |
 *         |                                                             |
 *         |-------------------------------------------------------------|
 *         |                                                             |
 *         |                   POOL Buffer Memory                        |
 *         |               Pool's buffers are created in this area       |
 *         |                                                             |
 *         |-------------------------------------------------------------|
 *
 *  ============================================================================
 */

/** ============================================================================
 *  @name   RESETCTRLADDR
 *
 *  @desc   Indicates the start address of Reset Ctrl memory region.
 *          last two nibbles must be zero i.e. align to 256 boundary.
 *  ============================================================================
 */
#define  RSTENTRYID         0
#define  RESETCTRLADDR      (DSP_REGION_BASE_ADDR + DSP_REGION_CMEM_SIZE)
#define  RESETCTRLSIZE      (0x00000080u)

/** ============================================================================
 *  @name   CODEMEMORYADDR/CODEMEMORYSIZE
 *
 *  @desc   Indicates startaddress/size for dsplink code region.
 *  ============================================================================
 */
#define  CODEENTRYID        1
#define  CODEMEMORYADDR     (RESETCTRLADDR + RESETCTRLSIZE)
#define  CODEMEMORYSIZE     (DSP_REGION_CODE_SIZE - RESETCTRLSIZE - SHAREDMEMORYSIZE0 - SHAREDMEMORYSIZE1 - POOLMEMORYSIZE)

/** ============================================================================
 *  @name   SHAREDENTRYID/SHAREDMEMORYADDR/SHAREDMEMORYSIZE
 *
 *  @desc   Indicates startaddress/size for dsplink shared memory region.
 *  ============================================================================
 */
#define  SHAREDENTRYID0     2
#define  SHAREDMEMORYADDR0  (CODEMEMORYADDR + CODEMEMORYSIZE)
#define  SHAREDMEMORYSIZE0  (0x00005000u)

/** ============================================================================
 *  @name   SHAREDENTRYID/SHAREDMEMORYADDR/SHAREDMEMORYSIZE
 *
 *  @desc   Indicates startaddress/size for dsplink shared memory region.
 *  ============================================================================
 */
#define  SHAREDENTRYID1     3
#define  SHAREDMEMORYADDR1  (SHAREDMEMORYADDR0 + SHAREDMEMORYSIZE0)
#define  SHAREDMEMORYSIZE1  (0x0002B000u)

/** ============================================================================
 *  @name   POOLMEMORYADDR/POOLMEMORYSIZE
 *
 *  @desc   Indicates startaddress/size for dsplink POOL memory region.
 *  ============================================================================
 */
#define  POOLENTRYID        4
#define  POOLMEMORYADDR     (SHAREDMEMORYADDR1 + SHAREDMEMORYSIZE1)
#define  POOLMEMORYSIZE     (0x000D0000u)


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

/** ============================================================================
 *  @name   LINKCFG_memTable_00
 *
 *  @desc   Memory table ID 0.
 *  ============================================================================
 */
STATIC LINKCFG_MemEntry myMemTable [] =
{
    {
        RSTENTRYID,                        /* ENTRY          : Entry number */
        "RESETCTRL",                       /* NAME           : Name of the memory region */
        RESETCTRLADDR,                     /* ADDRPHYS       : Physical address */
        RESETCTRLADDR,                     /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,                       /* ADDRGPPVIRT    : GPP virtual address (if known) */
        RESETCTRLSIZE,                     /* SIZE           : Size of the memory region */
        FALSE,                             /* SHARED         : Shared access memory? */
        FALSE,                             /* SYNCD          : Synchornized? */
    },
    {
        CODEENTRYID,                       /* ENTRY          : Entry number */
        "DDR",                             /* NAME           : Name of the memory region */
        CODEMEMORYADDR,                    /* ADDRPHYS       : Physical address */
        CODEMEMORYADDR,                    /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,                       /* ADDRGPPVIRT    : GPP virtual address (if known) */
        CODEMEMORYSIZE,                    /* SIZE           : Size of the memory region */
        FALSE,                             /* SHARED         : Shared access memory? */
        FALSE,                             /* SYNCD          : Synchornized? */
    },
    {
        SHAREDENTRYID0,                   /* ENTRY          : Entry number */
        "DSPLINKMEM",                     /* NAME           : Name of the memory region */
        SHAREDMEMORYADDR0,                /* ADDRPHYS       : Physical address */
        SHAREDMEMORYADDR0,                /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,                      /* ADDRGPPVIRT    : GPP virtual address (if known) */
        SHAREDMEMORYSIZE0,                /* SIZE           : Size of the memory region */
        TRUE,                             /* SHARED         : Shared access memory? */
        FALSE,                            /* SYNCD          : Synchornized? */
    },
    {
        SHAREDENTRYID1,                   /* ENTRY          : Entry number */
        "DSPLINKMEM1",                    /* NAME           : Name of the memory region */
        SHAREDMEMORYADDR1,                /* ADDRPHYS       : Physical address */
        SHAREDMEMORYADDR1,                /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,                      /* ADDRGPPVIRT    : GPP virtual address (if known) */
        SHAREDMEMORYSIZE1,                /* SIZE           : Size of the memory region */
        TRUE,                             /* SHARED         : Shared access memory? */
        FALSE,                            /* SYNCD          : Synchornized? */
    },
    {
        POOLENTRYID,                       /* ENTRY          : Entry number */
        "POOLMEM",                         /* NAME           : Name of the memory region */
        POOLMEMORYADDR,                    /* ADDRPHYS       : Physical address */
        POOLMEMORYADDR,                    /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,                       /* ADDRGPPVIRT    : GPP virtual address (if known) */
        POOLMEMORYSIZE,                    /* SIZE           : Size of the memory region */
        TRUE,                              /* SHARED         : Shared access memory? Logically */
        FALSE,                             /* SYNCD          : Synchornized? */
    },
    {
        5,                     /* ENTRY          : Entry number */
        "DSPIRAM",             /* NAME           : Name of the memory region */
        0x11800000,            /* ADDRPHYS       : Physical address */
        0x11800000,            /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,           /* ADDRGPPVIRT    : GPP virtual address (if known) */
        0x40000,               /* SIZE           : Size of the memory region */
        FALSE,                 /* SHARED         : Shared access memory? */
        FALSE,                 /* SYNCD          : Synchornized? */
    },
    {
        6,                     /* ENTRY          : Entry number */
        "DSPL1DRAM",           /* NAME           : Name of the memory region */
        0x11F00000,            /* ADDRPHYS       : Physical address */
        0x11F00000,            /* ADDRDSPVIRT    : DSP virtual address */
        (Uint32) -1,           /* ADDRGPPVIRT    : GPP virtual address (if known) */
        0x8000,                /* SIZE           : Size of the memory region */
        FALSE,                 /* SHARED         : Shared access memory? */
        FALSE,                 /* SYNCD          : Synchornized? */
    },
} ;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

/** ============================================================================
 *  @name   LINKCFG_memTables
 *
 *  @desc   Array of memory tables in the system.
 *  ============================================================================
 */
LINKCFG_MemEntry * myMemTables [] =
{
  myMemTable        /* Memory Table 0 */
};

/** ============================================================================
 *  @name   LINKCFG_dspObjects
 *
 *  @desc   Array of configuration objects for the DSPs in the system.
 *  ============================================================================
 */
LINKCFG_Dsp myDspObject =
{
    "DA8XXGEM",                                              /* NAME           : Name of the DSP */
    DspArch_C64x,                                            /* ARCHITECTURE   : DSP architecture */
    "COFFMEM",                                               /* LOADERNAME     : Name of the DSP executable loader */
    FALSE,                                                   /* AUTOSTART      : Autostart the DSP (Not supported) */
    "DEFAULT.OUT",                                           /* EXECUTABLE     : Executable for autostart */
    DSP_BootMode_Boot_Pwr,                                   /* DOPOWERCTRL    : Link does the Power Ctrl of DSP. */
    RESETCTRLADDR,                                           /* RESUMEADDR     : Resume address */
    RESETCTRLADDR,                                           /* RESETVECTOR    : Reset Vector for the DSP */
    RESETCTRLSIZE,                                           /* RESETCODESIZE  : Size of code at DSP Reset Vector */
    1,                                                       /* MADUSIZE       : DSP Minimum Addressable Data Unit */
    (Uint32) -1,                                             /* CPUFREQ        : DSP Frequency (in KHz), -1 if default setting is to be used */
    Endianism_Little,                                        /* ENDIAN         : DSP Endianism */
    FALSE,                                                   /* WORDSWAP       : Words must be swapped when writing to memory */
    0,                                                       /* MEMTABLEID     : ID of the memory table used */
    sizeof (myMemTable)/sizeof (LINKCFG_MemEntry),           /* MEMENTRIES     : Number of entries in memory table */
    0,                                                       /* LINKDRVID      : ID of the link driver used */
    0,                                                       /* ARG1           : Bus No of PCI card */
    0,                                                       /* ARG2           : Slot No of PCI card */
    0,                                                       /* ARG3           : DSPLINK Shared memory region for control data structures creation */
    0,                                                       /* ARG4           : Unused */
    0                                                        /* ARG5           : Unused */
} ;


/************************************************************
* Global Function Definitions                               *
************************************************************/


/************************************************************
* End file                                                  *
************************************************************/

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


