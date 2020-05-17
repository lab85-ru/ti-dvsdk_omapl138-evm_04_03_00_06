/** ============================================================================
 *  @file   dm648_hal_pci.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM648/
 *
 *  @desc   Hardware Abstraction Layer for PC-DM648 PCI interface.
 *          Defines interfaces to initialize the PCI interface.
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


#if !defined (DM648_HAL_PCI_H)
#define DM648_HAL_PCI_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm648_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* if defined (__cplusplus) */


/** ============================================================================
 *  @const  NUM_BARS
 *
 *  @desc   Number of BAR registers.
 *  ============================================================================
 */
#define NUM_BARS                    6

/** ============================================================================
 *  @const  PCI33_DMA_MAXTHROUGHPUT
 *
 *  @desc   Maximum through put of PCI interface.
 *  ============================================================================
 */
#define PCI33_DMA_MAXTHROUGHPUT     132u

/** ============================================================================
 *  @const   SCRATCH_BAR_NUMBER
 *
 *  @desc    BAR number for scratch.
 *  ============================================================================
 */
#define SCRATCH_BAR_NUMBER              0

/** ============================================================================
 *  @const  DDR_REGS_BAR_NO
 *
 *  @desc   Number for the BAR register used for DDR EMIF register access.
 *  ============================================================================
 */
#define DDR_REGS_BAR_NO             1u


/** ============================================================================
 *  @const  CFG_REGS_BAR_NO
 *
 *  @desc   Number for the BAR register used for register access.
 *  ============================================================================
 */
#define CFG_REGS_BAR_NO             2u

/** ============================================================================
 *  @const  RWMEM_BAR_NO
 *
 *  @desc   Number for the BAR register used for L1DRAM access.
 *  ============================================================================
 */
#define RWMEM_BAR_NO                4u

/** ============================================================================
 *  @const  SHMEM_BAR_NO
 *
 *  @desc   Number for the BAR register used for shared memory access.
 *  ============================================================================
 */
#define SHMEM_BAR_NO                5u

/** ============================================================================
 *  @const   LPSC_GEM
 *
 *  @desc    Module number for GEM.
 *  ============================================================================
 */
#define LPSC_GEM                    33u

/** ============================================================================
 *  @const   LPSC_EDMA_TPCC
 *
 *  @desc    Module number for EDMA TPCC.
 *  ============================================================================
 */
#define LPSC_EDMA_TPCC              0u

/** ============================================================================
 *  @const   LPSC_DDR
 *
 *  @desc    Module number for DDR.
 *  ============================================================================
 */
#define LPSC_DDR                    7u

/** ============================================================================
 *  @const   DM648_BAR2_BASE
 *
 *  @desc    Default value contained in BAR2.
 *  ============================================================================
 */
#define DM648_BAR2_BASE              0x02000000u

/** ============================================================================
 *  @const   DM648_PCIMEM_BASE
 *
 *  @desc    PCI memory base in GEM memory space.
 *  ============================================================================
 */
#define DM648_PCIMEM_BASE          0x40000000u

/** ============================================================================
 *  @const   DM6437_DEVREG_BASE
 *
 *  @desc    Base address of Device config registers.
 *  ============================================================================
 */
#define DM648_DEVREG_BASE          0x02049000u

/** ============================================================================
 *  @const   DM648_EDMAREG_BASE
 *
 *  @desc    Base address of EDMA registers.
 *  ============================================================================
 */
#define DM648_EDMAREG_BASE         0x02A00000u

/** ============================================================================
 *  @const   DM648_DDRREG_BASE
 *
 *  @desc    Base address of DDR PHY registers.
 *  ============================================================================
 */
#define DM648_DDRREG_BASE          0x78000000u

/** ============================================================================
 *  @const   DM648_MDCTL_BASE
 *
 *  @desc    Base address of MDCTL registers.
 *  ============================================================================
 */
#define DM648_MDCTL_BASE             0x02046A00

/** ============================================================================
 *  @const   DM648_MDSTAT_BASE
 *
 *  @desc    Base address of MDSTAT registers.
 *  ============================================================================
 */
#define DM648_MDSTAT_BASE        0x02046800

/** ============================================================================
 *  @const   DM648_PTCMD_BASE
 *
 *  @desc    Base address of MDCTL register.
 *  ============================================================================
 */
#define DM648_PTCMD_BASE             0x02046120

/** ============================================================================
 *  @const   DM648_PTSTAT_BASE
 *
 *  @desc    Base address of MDCTL registers.
 *  ============================================================================
 */
#define DM648_PTSTAT_BASE        0x02046128

/** ============================================================================
 *  @const   DM648_PCIREG_BASE
 *
 *  @desc    Base address of PCI backend registers.
 *  ============================================================================
 */
#define DM648_PCIREG_BASE          0x02048400u


/** ============================================================================
 *  @const   DM648_SOFTINT0_MASK
 *
 *  @desc    Mask for generating soft int0 (DSP->GPP)
 *  ============================================================================
 */
#define DM648_SOFTINT0_MASK        0x01000000u

/** ============================================================================
 *  @const   DM648_SOFTINT1_MASK
 *
 *  @desc    Mask for generating soft int1 (GPP->DSP)
 *  ============================================================================
 */
#define DM648_SOFTINT1_MASK        0x02000000

/** ============================================================================
 *  @const   DM648_LRESET_MASK
 *
 *  @desc    Mask for reseting/releasing GEM.
 *  ============================================================================
 */
#define DM648_LRESET_MASK          0x00000100u

/** ============================================================================
 *  @const   DM648_INTSTATUS_MASK
 *
 *  @desc    Bitmask for Interrupt status (DSP->GPP)
 *  ============================================================================
 */
#define DM648_INTSTATUS_MASK       0x00080000u

/** ============================================================================
 *  @const   DM648_BOOTCMPLTBC_MASK
 *
 *  @desc    Bitmask for Setting BC bit in BOOTCOMPLT register.
 *  ============================================================================
 */
#define DM648_BOOTCMPLTBC_MASK     0x00000001u

/** ============================================================================
 *  @const   DM648_PCIADLEN
 *
 *  @desc    Length each segment of addressable PCI Space..
 *  ============================================================================
 */
#define DM648_PCIADLEN             0x00800000u

/** ============================================================================
 *  @const   DM648_PCIADWRBITMASK
 *
 *  @desc    Mask indicating writeable bits in PCI Address Window registers.
 *  ============================================================================
 */
#define DM648_PCIADWRBITMASK       0xFF800000u

/** ============================================================================
 *  @const   DM648_PAGEWRBITMASK
 *
 *  @desc    Mask indicating writeable bits in PCI Base Address Mask Register5.
 *  ============================================================================
 */
#define DM648_PAGEWRBITMASK       0xFF800000

/** ============================================================================
 *  @const   HAL_CONFIGURE_MAP
 *
 *  @desc    Value indicating mapping has to be done.
 *  ============================================================================
 */
#define HAL_CONFIGURE_MAP               0x1

/** ============================================================================
 *  @const   HAL_CONFIGURE_UNMAP
 *
 *  @desc    Value indicating unmapping has to be done.
 *  ============================================================================
 */
#define HAL_CONFIGURE_UNMAP             0x2

/** ============================================================================
 *  @const   HAL_CONFIGURE_SETP
 *
 *  @desc    Value indicating simply set the dsp for the given address.
 *  ============================================================================
 */
#define HAL_CONFIGURE_SET               0x3

/** ============================================================================
 *  @name   DM6437_devRegs
 *
 *  @desc   Register Overlay Structure for Device config registers.
 *  ============================================================================
 */
/** ============================================================================
 *  @name   DM648_devRegs
 *
 *  @desc   Register Overlay Structure for Device config registers.
 *  ============================================================================
 */
typedef struct DM648_devRegs_tag {
    volatile Uint32 PINMUX0       ;
    volatile Uint32 PINMUX1       ;
    volatile Uint32 DSPBOOTADDR   ;
    volatile Uint32 BOOTCMPLT     ;
    volatile Uint32 RSVD0         ;
    volatile Uint32 BOOTCFG       ;
    volatile Uint32 VDD1P0V_ADJ   ;
    volatile Uint32 VDD1P2V_ADJ   ;
    volatile Uint32 DDR_SLEW      ;
    volatile Uint32 PERIPHEN      ;
    volatile Uint32 DEVICE_ID     ;
    volatile Uint32 DAC_DEMEN     ;
    volatile Uint32 UHPICTL       ;
    volatile Uint32 RSVD1 [2]     ;
    volatile Uint32 MSTPRI0       ;
    volatile Uint32 MSTPRI1       ;
    volatile Uint32 VPSS_CLK_CTRL ;
    volatile Uint32 VDD3P3V_PWRDN ;
    volatile Uint32 DFT_ENABLE    ;
    volatile Uint32 SEC_SCAN_REG  ;
    volatile Uint32 SEC_TEST_REG  ;
    volatile Uint32 SEC_TAP_CTL   ;
    volatile Uint32 PUBLIC_KEY0   ;
    volatile Uint32 PUBLIC_KEY1   ;
    volatile Uint32 PUBLIC_KEY2   ;
    volatile Uint32 PUBLIC_KEY3   ;
    volatile Uint32 VLYNQ_DELAY   ;
    volatile Uint32 SEC_PSEUDO0   ;
    volatile Uint32 SEC_PSEUDO1   ;
    volatile Uint32 RSVD2 [3]     ;
    volatile Uint32 TIMERCTL      ;
    volatile Uint32 TPTCCCFG      ;
    volatile Uint32 RSVD3         ;
} DM648_devRegs ;

/** ============================================================================
 *  @name   DM648_ddrRegs
 *
 *  @desc   Register Overlay Structure for DDR.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 MIDR        ;
    volatile Uint32 SDSTAT      ;
    volatile Uint32 SDCFG       ;
    volatile Uint32 SDREF       ;
    volatile Uint32 SDTIM1      ;
    volatile Uint32 SDTIM2      ;
    volatile Uint8  RSVD0[8]    ;
    volatile Uint32 BPRIO       ;
    volatile Uint8  RSVD1 [192]  ;
    volatile Uint32 DDRCTL      ;
    volatile Uint32 PHYSTAT     ;
} DM648_ddrRegs ;

/** ============================================================================
 *  @name   DM648_paramEntry
 *
 *  @desc   Register Overlay Structure for PARAMENTRY.
 *  ============================================================================
 */
typedef struct DM648_paramEntry_tags {
    volatile Uint32 OPTION        ;
    volatile Uint32 SRC           ;
    volatile Uint32 A_B_CNT       ;
    volatile Uint32 DST           ;
    volatile Uint32 SRC_DST_BIDX  ;
    volatile Uint32 LINK_BCNTRLD  ;
    volatile Uint32 SRC_DST_CIDX  ;
    volatile Uint32 CCNT          ;
} DM648_paramEntry ;

/** ============================================================================
 *  @name   DM648_edmaRegs
 *
 *  @desc   Register Overlay Structure for EDMA.
 *  ============================================================================
 */
typedef struct DM648_edmaRegs_tag {
    volatile Uint32      REV                ;
    volatile Uint32      CCCFG              ;
    volatile Uint8       RSVD0 [248]        ;
    volatile Uint32      DCHMAP [64]        ;
    volatile Uint32      QCHMAP [8]         ;
    volatile Uint8       RSVD1 [32]         ;
    volatile Uint32      DMAQNUM [8]        ;
    volatile Uint32      QDMAQNUM           ;
    volatile Uint8       RSVD2 [28]         ;
    volatile Uint32      QUETCMAP           ;
    volatile Uint32      QUEPRI             ;
    volatile Uint8       RSVD3 [120]        ;
    volatile Uint32      EMR                ;
    volatile Uint32      EMRH               ;
    volatile Uint32      EMCR               ;
    volatile Uint32      EMCRH              ;
    volatile Uint32      QEMR               ;
    volatile Uint32      QEMCR              ;
    volatile Uint32      CCERR              ;
    volatile Uint32      CCERRCLR           ;
    volatile Uint32      EEVAL              ;
    volatile Uint8       RSVD4  [28]        ;
    volatile Uint8       RSVD5 [64]         ;
    volatile Uint32      QRAE [8]           ;
    volatile Uint8       RSVD6[96]          ;
    volatile Uint8       RSVD7 [512]        ;
    volatile Uint32      QSTAT [8]          ;
    volatile Uint32      QWMTHRA            ;
    volatile Uint32      QWMTHRB            ;
    volatile Uint8       RSVD8 [24]         ;
    volatile Uint32      CCSTAT             ;
    volatile Uint8       RSVD9 [188]        ;
    volatile Uint32      AETCTL             ;
    volatile Uint32      AETSTAT            ;
    volatile Uint32      AETCMD             ;
    volatile Uint8       RSVD10 [244]       ;
    volatile Uint32      MPFAR              ;
    volatile Uint32      MPFSR              ;
    volatile Uint32      MPFCR              ;
    volatile Uint32      MPPAG              ;
    volatile Uint32      MPPA [8]           ;
    volatile Uint8       RSVD11 [2000]      ;
    volatile Uint32      ER                 ;
    volatile Uint32      ERH                ;
    volatile Uint32      ECR                ;
    volatile Uint32      ECRH               ;
    volatile Uint32      ESR                ;
    volatile Uint32      ESRH               ;
    volatile Uint32      CER                ;
    volatile Uint32      CERH               ;
    volatile Uint32      EER                ;
    volatile Uint32      EERH               ;
    volatile Uint32      EECR               ;
    volatile Uint32      EECRH              ;
    volatile Uint32      EESR               ;
    volatile Uint32      EESRH              ;
    volatile Uint32      SER                ;
    volatile Uint32      SERH               ;
    volatile Uint32      SECR               ;
    volatile Uint32      SECRH              ;
    volatile Uint8       RSVD12 [8]         ;
    volatile Uint32      IER                ;
    volatile Uint32      IERH               ;
    volatile Uint32      IECR               ;
    volatile Uint32      IECRH              ;
    volatile Uint32      IESR               ;
    volatile Uint32      IESRH              ;
    volatile Uint32      IPR                ;
    volatile Uint32      IPRH               ;
    volatile Uint32      ICR                ;
    volatile Uint32      ICRH               ;
    volatile Uint32      IEVAL              ;
    volatile Uint8       RSVD13 [4]         ;
    volatile Uint32      QER                ;
    volatile Uint32      QEER               ;
    volatile Uint32      QEECR              ;
    volatile Uint32      QEESR              ;
    volatile Uint32      QSER               ;
    volatile Uint32      QSECR              ;
    volatile Uint8       RSVD14 [3944]      ;
    volatile Uint8       RSVD15 [4096]      ;
    volatile Uint8       RSVD16 [4096]      ;
    DM648_paramEntry     PARAMENTRY[512]    ;
} DM648_edmaRegs ;

/** ============================================================================
 *  @name   DM648_pciRegs
 *
 *  @desc   PCI Back end register overlay structure.
 *  ============================================================================
 */
typedef struct DM648_pciRegs_tag {
    volatile Uint32 PCIREVID       ;
    volatile Uint8  RSVD0 [12]     ;
    volatile Uint32 PCISTATSET     ;
    volatile Uint32 PCISTATCLR     ;
    volatile Uint8  RSVD1 [8]      ;
    volatile Uint32 PCIHINTSET     ;
    volatile Uint32 PCIHINTCLR     ;
    volatile Uint8  RSVD2 [8]      ;
    volatile Uint32 PCIBINTSET     ;
    volatile Uint32 PCIBINTCLR     ;
    volatile Uint32 PCIBCLKMGT     ;
    volatile Uint8  RSVD3 [196]    ;
    volatile Uint32 PCIVENDEVMIR   ;
    volatile Uint32 PCICSRMIR      ;
    volatile Uint32 PCICLREVMIR    ;
    volatile Uint32 PCICLINEMIR    ;
    volatile Uint32 PCIBAR0MSK     ;
    volatile Uint32 PCIBAR1MSK     ;
    volatile Uint32 PCIBAR2MSK     ;
    volatile Uint32 PCIBAR3MSK     ;
    volatile Uint32 PCIBAR4MSK     ;
    volatile Uint32 PCIBAR5MSK     ;
    volatile Uint8  RSVD4[4]       ;
    volatile Uint32 PCISUBIDMIR    ;
    volatile Uint8  RSVD5 [4]      ;
    volatile Uint32 PCICPBPTRMIR   ;
    volatile Uint8  RSVD6 [4]      ;
    volatile Uint32 PCILGINTMIR    ;
    volatile Uint8  RSVD7 [64]     ;
    volatile Uint32 PCISLVCNTL     ;
    volatile Uint8  RSVD8 [60]     ;
    volatile Uint32 PCIBAR0TRL     ;
    volatile Uint32 PCIBAR1TRL     ;
    volatile Uint32 PCIBAR2TRL     ;
    volatile Uint32 PCIBAR3TRL     ;
    volatile Uint32 PCIBAR4TRL     ;
    volatile Uint32 PCIBAR5TRL     ;
    volatile Uint8  RSVD9 [8]      ;
    volatile Uint32 PCIBARMIR [6]  ;
    volatile Uint8  RSVD10 [264]   ;
    volatile Uint32 PCIMCFGDAT     ;
    volatile Uint32 PCIMCFGADR     ;
    volatile Uint32 PCIMCFGCMD     ;
    volatile Uint8  RSVD11 [4]     ;
    volatile Uint32 PCIMSTCFG      ;
    volatile Uint32 PCIADDSUB [32] ;
    volatile Uint32 PCIVENDEVPRG   ;
    volatile Uint32 PCICMDSTATPRG  ;
    volatile Uint32 PCICLREVPRG    ;
    volatile Uint32 PCISUBIDPRG    ;
    volatile Uint32 PCIMAXLGPRG    ;
    volatile Uint32 PCILRSTREG     ;
    volatile Uint32 PCICFGDONE     ;
    volatile Uint32 PCIBAR0MPRG    ;
    volatile Uint32 PCIBAR1MPRG    ;
    volatile Uint32 PCIBAR2MPRG    ;
    volatile Uint32 PCIBAR3MPRG    ;
    volatile Uint32 PCIBAR4MPRG    ;
    volatile Uint32 PCIBAR5MPRG    ;
    volatile Uint32 PCIBAR0PRG     ;
    volatile Uint32 PCIBAR1PRG     ;
    volatile Uint32 PCIBAR2PRG     ;
    volatile Uint32 PCIBAR3PRG     ;
    volatile Uint32 PCIBAR4PRG     ;
    volatile Uint32 PCIBAR5PRG     ;
    volatile Uint32 PCIBAR0TRLPRG  ;
    volatile Uint32 PCIBAR1TRLPRG  ;
    volatile Uint32 PCIBAR2TRLPRG  ;
    volatile Uint32 PCIBAR3TRLPRG  ;
    volatile Uint32 PCIBAR4TRLPRG  ;
    volatile Uint32 PCIBAR5TRLPRG  ;
    volatile Uint32 PCIBASENPRG    ;
} DM648_pciRegs ;


/** ============================================================================
 *  @name   DM648_HalPciPhyObj_tag
 *
 *  @desc   Physical Interface object.
 *
 *  @field  dev
 *              Pointer to Linux struct for pci device .
 *  @field  busNo
 *              BUS number for PCI card .
 *  @field  slotNo
 *              SLOT number for PCI card .
 *  @field  prevPageAddr
 *              Stores previous page base address .
 *  @field  shMemAddr
 *              Shared memory base address .
 *  @field  barPhys
 *              Physical address of BAR areas .
 *  @field  barLen
 *              Length of BAR areas .
 *  @field  barVirt
 *              Virtual address of BAR areas .
 *  @field  edmaChnlId
 *              EDMA Channel number.
 *  ============================================================================
 */
struct DM648_HalPciPhyObj_tag {
    Pvoid       dev                ;
    Uint32      irqId              ;
    Uint32      busNo              ;
    Uint32      slotNo             ;
    Uint32      prevPageAddr       ;
    Uint32      shMemDspAddr       ;
    Uint32      shMemVirtAddr      ;
    Uint32      shMemPhysAddr      ;
    Uint32      barPhys [NUM_BARS] ;
    Uint32      barLen  [NUM_BARS] ;
    Uint32      barVirt [NUM_BARS] ;
    Uint32      edmaChnlId         ;
} ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM648_HAL_PCI_H) */
