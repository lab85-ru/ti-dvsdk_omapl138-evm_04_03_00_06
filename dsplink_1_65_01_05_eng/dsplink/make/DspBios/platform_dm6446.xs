/** ============================================================================
 *  @file   platform_dm6446.xs
 *
 *  @path   $(DSPLINK)/make/DspBios/
 *
 *   @desc   This makefile defines OS specific macros used by MAKE system for
 *           the DSP/BIOS version 5.xx for C64XX PLUS on Windows.
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



var memory = [];


memory[5] = ["DDR2",
    {
        name: "DDR2",
        base: 0x8fe00080,
        len: 0xFFF80,
        space: "code/data"
    }];
memory[1] = ["CACHE_L2",
    {
        name: "CACHE_L2",
        base: 0x11808000,
        len: 0x8000,
        space: "Cache"
    }];

memory[2] = ["ARM_RAM",
    {
        name: "ARM_RAM",
        base: 0x10008000,
        len: 0x4000,
        space: "data"
    }];

memory[3] = ["IRAM",
    {
        name: "IRAM",
        base: 0x11800000,
        len: 0x8000,
        space: "code/data"
    }];

memory[4] = ["CACHE_L1D",
    {
        name: "CACHE_L1D",
        base: 0x11f10000,
        len: 0x8000,
        space: "Cache"
    }];

memory[0] = ["DSPLINKMEM",
    {
        name: "DSPLINKMEM",
        base: 0x8ff00000,
        len: 0x30000,
        space: "data"
    }];

memory[6] = ["CACHE_L1P",
    {
        name: "CACHE_L1P",
        base: 0x11e08000,
        len: 0x8000,
        space: "Cache"
    }];

memory[7] = ["L1DSRAM",
    {
        name: "L1DSRAM",
        base: 0x11f04000,
        len: 0xc000,
        space: "data"
    }];

memory[8] = ["RESET_VECTOR",
    {
        name: "RESET_VECTOR",
        base: 0x8FE00000,
        len: 0x80,
        space: "code/data"
    }];

memory[9] = ["POOL_MEM",
    {
        name: "POOL_MEM",
        base: 0x8ff30000,
        len: 0xD0000,
        space: "data"
    }];

Build.platformTable['ti.platforms.generic:plat'] = {
    clockRate: 450,
    catalogName: 'ti.catalog.c6000',
    deviceName: 'TMS320CDM420',
    l2Mode: '32k',
    customMemoryMap: memory
};

