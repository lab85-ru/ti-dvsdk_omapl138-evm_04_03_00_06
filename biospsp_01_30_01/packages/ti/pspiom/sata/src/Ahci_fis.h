/*
 * ahci_fis.h
 *
 * contain the AHCI specific Frame Information Structures (FIS)
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
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

#include "tistdtypes.h"
/**
 * \brief RegFis
 *
 * Register FIS - H2D_FIS, D2H_FIS
 */
typedef struct {
	/** DW0:00 flags, fistype, status, error */
	Uint8	type;
	Uint8	flags;
#define REGFIS_FLAGS_CMD	(1 << 7)
	union{
		Uint8	command;
		Uint8	status;
	}u1;
	union {
		Uint8	Error;
		Uint8	features;
	}u2;
	/** DW1:04 Device,lbaHigh,lbaMid,lbaLow */
	Uint8	lbaLow;
	Uint8	lbaMid;
	Uint8	lbaHigh;
	Uint8	Device;
	/** DW2:08 resr,lbaHigh_exp,lbaMid_exp,lbaLow_exp */
	Uint8	lbaLow_exp;
	Uint8	lbaMid_exp;
	Uint8	lbaHigh_exp;
	Uint8	reserved1;
	/** DW3:0c rerved, sector count*/
	Uint16	sector_count;
	Uint8	reserved2;
	Uint8	control;
	/* DW4:10 reserved */
	Uint32	reserved3;
}RegFis;

/**
 * \brief PioSetupFis
 *
 * PIO Setup Fis
 */
typedef struct {
	/** DW0:00 flags, fistype, status, error */
	Uint8	fis_type;
	Uint8	flags;
	Uint8	status;
	Uint8	Error;
	/** DW1:04 Device,lbaHigh,lbaMid,lbaLow */
	Uint8	lbaLow;
	Uint8	lbaMid;
	Uint8	lbaHigh;
	Uint8	Device;
	/** DW2:08 resr,lbaHigh_exp,lbaMid_exp,lbaLow_exp */
	Uint8	lbaLow_exp;
	Uint8	lbaMid_exp;
	Uint8	lbaHigh_exp;
	Uint8	reserved1;
	/** DW3:0C rerved, sector count*/
	Uint16	sector_count;
	Uint8	reserved2;
	Uint8	E_Status;
	/* DW4:10 reserved */
	Uint16	transfer_count;
	Uint16	reserved3;
}PioSetupFis;

/**
 * \brief DmaSetupFis
 *
 * DMA Setup FIS
 */
typedef struct {
	/** DW0:00 res,res,flags(AIDR-PMPORT),FIStype */
	Uint8	fis_type;
	Uint8	flags;
	Uint16	reserved1;
	/** DW1:04 DMA Buffer Identifier Low */
	Uint32	dmabuf_low;
	/** DW2:08 DMA Buffer Identifier High */
	Uint32	dmabuf_high;
	/** DW3:0C reserved */
	Uint32	reserved2;
	/* DW4:10 DMA buffer offset */
	Uint32	dmabuf_offset;
	/* DW5:14 DMA Transfer count */
	Uint32	dma_transfer_count;
	/* DW6:18 reserved */
	Uint32	reserved3;
}DmaSetupFis;

/**
 * \brief SetDevBitFis
 *
 * SetDevice Bits FIS
 */
typedef struct {
	/** DW0:00 error, (R,stats_hi,stats_low),flags(I,PMP),fistype */
	Uint8	fis_type;
	Uint8	flags;
	Uint8	status;
	Uint8	Error;
	/** DW1:04 reserved */
	Uint32	reserved;
}SetDevBitFis;

/**
 * \brief DmaActivateFis
 *
 * DMA Activate FIS
 */

typedef struct {
	/** DW0:00 res,res,flags(PMP),fistype */
	Uint8	fis_type;
	Uint8	flags;
	Uint16	reserved;
}DmaActivateFis;

/**
 * \brief BistActivateFis
 *
 * BIST Activate FIS
 */
typedef struct {
	/** DW0:00 res,Pattern,flags(PMP),fistype */
	Uint8	fis_type;
	Uint8	flags;
	Uint8	pattern;
	Uint8	reserved;
	/** DW0:04 Data1 pattaern */
	Uint32	Data1;
	/** DW0:08 Data2 pattern */
	Uint32	Data2;
}BistActivateFis;

 /**
 * \brief AhciRxFis
 *
 * Memory map receive FIS structure
 * size is 256 bytes
 */
typedef struct {
	/** 00-1F: DMA Setup FIS */
	DmaSetupFis dmaSetupFis;
	Uint32 reserved1;
	/** 20-3F: PIO Setup FIS */
	PioSetupFis pioSetupFis;
	Uint32 reserved2[3];
	/** 40-57: D2H Register FIS */
	RegFis	D2HRegFis;
	Uint32 reserved3;
	/** 58-5c: SetupDevice Bits FIS */
	SetDevBitFis setDevBitFis;
	/** 60-A0: Unknow FIS */
	Uint32	UnknownFis[16];
	/** A0-FF: reserved */
	Uint32	reserved5[24];
}AhciRxFis;

