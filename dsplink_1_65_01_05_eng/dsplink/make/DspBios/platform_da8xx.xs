/** ============================================================================
 *  @file   platform_da8xx.xs
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

memory[0] = ["L3_CBA_RAM",
	{
		name		: "L3_CBA_RAM",
		base		: 0x80000000,
		len			: 0x20000,
		space		: "code/data"
	}];

memory[1] = ["DSPLINKMEM",
	{
		name		: "DSPLINKMEM",
		base		: 0xC3F00000,
		len			: 0x30000,
		space		: "data"
	}];

memory[2] = ["CACHE_L2",
	{
		name		: "CACHE_L2",
		base		: 0x11808000,
		len			: 0x30000,
		space		: "Cache"
	}];

memory[3] = ["IRAM",
	{
		name		: "IRAM",
		base		: 0x11800000,
		len			: 0x8000,
		space		: "code/data"
	}];

memory[4] = ["POOLMEM",
	{
		name		: "POOLMEM",
		base		: 0xC3F30000,
		len			: 0xd0000,
		space		: "data"
	}];

memory[5] = ["CACHE_L1D",
	{
		name		: "CACHE_L1D",
		base		: 0x11f00000,
		len			: 0x8000,
		space		: "Cache"
	}];

memory[6] = ["CACHE_L1P",
	{
		name		: "CACHE_L1P",
		base		: 0x11e00000,
		len			: 0x8000,
		space		: "Cache"
	}];

memory[7] = ["RESET_VECTOR",
	{
		name    	: "RESET_VECTOR",
		base      	: 0xC3E00000,
		len      	: 0x80,
		space     	: "code/data"
	}];

memory[8] = ["SDRAM",
	{
		name      	: "SDRAM",
		base      	: 0xC3E00080,
		len       	: 0xfff80,
		space     	: "code/data",
	}];

Build.platformTable['ti.platforms.generic:plat'] = {
	clockRate: 300,
	catalogName: 'ti.catalog.c6000',
	deviceName: 'TMS320DA830',
	l2Mode: '256k',
	customMemoryMap: memory
};

