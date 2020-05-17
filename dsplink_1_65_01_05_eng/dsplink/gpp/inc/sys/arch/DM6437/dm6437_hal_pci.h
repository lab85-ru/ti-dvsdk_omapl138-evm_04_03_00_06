/** ============================================================================
 *  @file   dm6437_hal_pci.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/DM6437/
 *
 *  @desc   Hardware Abstraction Layer for PC-DM64LC PCI interface.
 *          Defines interfaces to initialize the PCI interface.
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


#if !defined (DM6437_HAL_PCI_H)
#define DM6437_HAL_PCI_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>


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
#define LPSC_GEM                    39u

/** ============================================================================
 *  @const   LPSC_EDMA_TPCC
 *
 *  @desc    Module number for EDMA TPCC.
 *  ============================================================================
 */
#define LPSC_EDMA_TPCC              2u

/** ============================================================================
 *  @const   LPSC_EDMA_TPTC0
 *
 *  @desc    Module number for EDMA TPTC0.
 *  ============================================================================
 */
#define LPSC_EDMA_TPTC0             3u

/** ============================================================================
 *  @const   LPSC_EDMA_TPTC1
 *
 *  @desc    Module number for EDMA TPTC1.
 *  ============================================================================
 */
#define LPSC_EDMA_TPTC1             4u

/** ============================================================================
 *  @const   LPSC_EDMA_TPTC2
 *
 *  @desc    Module number for EDMA TPTC2.
 *  ============================================================================
 */
#define LPSC_EDMA_TPTC2             5u

/** ============================================================================
 *  @const   LPSC_DDR
 *
 *  @desc    Module number for DDR.
 *  ============================================================================
 */
#define LPSC_DDR                    13u

/** ============================================================================
 *  @const   DM6437_PCIMEM_BASE
 *
 *  @desc    PCI memory base in GEM memory space.
 *  ============================================================================
 */
#define DM6437_PCIMEM_BASE          0x30000000u

/** ============================================================================
 *  @const   DM6437_DEVREG_BASE
 *
 *  @desc    Base address of Device config registers.
 *  ============================================================================
 */
#define DM6437_DEVREG_BASE          0x40000u

/** ============================================================================
 *  @const   DM6437_PLLREG_BASE
 *
 *  @desc    Base address of PLL0 registers.
 *  ============================================================================
 */
#define DM6437_PLL0REG_BASE         0x40800u

/** ============================================================================
 *  @const   DM6437_PLLREG_BASE
 *
 *  @desc    Base address of PLL1 registers.
 *  ============================================================================
 */
#define DM6437_PLL1REG_BASE         0x40C00u

/** ============================================================================
 *  @const   DM6437_EDMAREG_BASE
 *
 *  @desc    Base address of EDMA registers.
 *  ============================================================================
 */
#define DM6437_EDMAREG_BASE         0x0u

/** ============================================================================
 *  @const   DM6437_DDRREG_BASE
 *
 *  @desc    Base address of DDR PHY registers.
 *  ============================================================================
 */
#define DM6437_DDRREG_BASE          0x0u

/** ============================================================================
 *  @const   DM6437_PSCREG_BASE
 *
 *  @desc    Base address of PSC registers.
 *  ============================================================================
 */
#define DM6437_PSCREG_BASE          0x41000u

/** ============================================================================
 *  @const   DM6437_PCIREG_BASE
 *
 *  @desc    Base address of PCI backend registers.
 *  ============================================================================
 */
#define DM6437_PCIREG_BASE          0x1A000u

/** ============================================================================
 *  @const   DM6437_SOFTINT0_MASK
 *
 *  @desc    Mask for generating soft int0 (DSP->GPP)
 *  ============================================================================
 */
#define DM6437_SOFTINT0_MASK        0x01000000u

/** ============================================================================
 *  @const   DM6437_SOFTINT1_MASK
 *
 *  @desc    Mask for generating soft int1 (GPP->DSP)
 *  ============================================================================
 */
#define DM6437_SOFTINT1_MASK        0x02000000u

/** ============================================================================
 *  @const   DM6437_LRESET_MASK
 *
 *  @desc    Mask for reseting/releasing GEM.
 *  ============================================================================
 */
#define DM6437_LRESET_MASK          0x00000100u

/** ============================================================================
 *  @const   DM6437_INTSTATUS_MASK
 *
 *  @desc    Bitmask for Interrupt status (DSP->GPP)
 *  ============================================================================
 */
#define DM6437_INTSTATUS_MASK       0x00080000u

/** ============================================================================
 *  @const   DM6437_BOOTCMPLTBC_MASK
 *
 *  @desc    Bitmask for Setting BC bit in BOOTCOMPLT register.
 *  ============================================================================
 */
#define DM6437_BOOTCMPLTBC_MASK     0x00000001u

/** ============================================================================
 *  @const   DM6437_PCIADLEN
 *
 *  @desc    Length each segment of addressable PCI Space..
 *  ============================================================================
 */
#define DM6437_PCIADLEN             0x00800000u

/** ============================================================================
 *  @const   DM6437_PCIADWRBITMASK
 *
 *  @desc    Mask indicating writeable bits in PCI Address Window registers.
 *  ============================================================================
 */
#define DM6437_PCIADWRBITMASK       0xFF800000u

/** ============================================================================
 *  @const   DM6437_PAGEWRBITMASK
 *
 *  @desc    Mask indicating writeable bits in PCI Base Address Mask Register5.
 *  ============================================================================
 */
#define DM6437_PAGEWRBITMASK        0xFF800000u


/** ============================================================================
 *  @name   DM6437_devRegs
 *
 *  @desc   Register Overlay Structure for Device config registers.
 *  ============================================================================
 */
typedef struct DM6437_devRegs_tag {
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
} DM6437_devRegs ;

/** ============================================================================
 *  @name   DM6437_pllRegs
 *
 *  @desc   Register Overlay Structure for PLL.
 *  ============================================================================
 */
typedef struct DM6437_pllRegs_tag {
    volatile Uint32 PID         ;
    volatile Uint8  RSVD0 [220] ;
    volatile Uint32 FUSERR      ;
    volatile Uint32 RSTYPE      ;
    volatile Uint32 RSTDEF      ;
    volatile Uint8  RSVD1 [20]  ;
    volatile Uint32 PLLCTL      ;
    volatile Uint32 OCSEL       ;
    volatile Uint32 SECCTL      ;
    volatile Uint8  RSVD2 [4]   ;
    volatile Uint32 PLLM        ;
    volatile Uint32 PREDIV      ;
    volatile Uint32 PLLDIV1     ;
    volatile Uint32 PLLDIV2     ;
    volatile Uint32 PLLDIV3     ;
    volatile Uint32 OSCDIV1     ;
    volatile Uint32 POSTDIV     ;
    volatile Uint32 BPDIV       ;
    volatile Uint32 WAKEUP      ;
    volatile Uint8  RSVD3 [4]   ;
    volatile Uint32 PLLCMD      ;
    volatile Uint32 PLLSTAT     ;
    volatile Uint32 ALNCTL      ;
    volatile Uint32 DCHANGE     ;
    volatile Uint32 CKEN        ;
    volatile Uint32 CKSTAT      ;
    volatile Uint32 SYSTAT      ;
    volatile Uint8  RSVD4 [12]  ;
    volatile Uint32 PLLDIV4     ;
    volatile Uint32 PLLDIV5     ;
    volatile Uint32 PLLDIV6     ;
    volatile Uint32 PLLDIV7     ;
    volatile Uint32 PLLDIV8     ;
} DM6437_pllRegs ;

/** ============================================================================
 *  @name   DM6437_ddrRegs
 *
 *  @desc   Register Overlay Structure for DDR.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 ERCSR       ;
    volatile Uint32 SDRSTAT     ;
    volatile Uint32 SDBCR       ;
    volatile Uint32 SDRCR       ;
    volatile Uint32 SDTIMR      ;
    volatile Uint32 SDTIMR2     ;
    volatile Uint8  RSVD0 [8]   ;
    volatile Uint32 VBPR        ;
    volatile Uint8  RSVD1 [4]   ;
    volatile Uint32 VBCFG1      ;
    volatile Uint32 VBCFG2      ;
    volatile Uint8  RSVD2 [16]  ;
    volatile Uint32 PERFC1R     ;
    volatile Uint32 PERFC2R     ;
    volatile Uint32 PCCR        ;
    volatile Uint32 PCMRSR      ;
    volatile Uint8  RSVD3 [48]  ;
    volatile Uint32 ASYNCCS2CR  ;
    volatile Uint32 ASYNCCS3CR  ;
    volatile Uint32 ASYNCCS4CR  ;
    volatile Uint32 ASYNCCS5CR  ;
    volatile Uint8  RSVD4 [16]  ;
    volatile Uint32 AWCCR       ;
    volatile Uint8  RSVD5 [28]  ;
    volatile Uint32 IRR         ;
    volatile Uint32 IMR         ;
    volatile Uint32 IMSR        ;
    volatile Uint32 IMCR        ;
    volatile Uint8  RSVD6 [16]  ;
    volatile Uint32 DDRPHYREV   ;
    volatile Uint32 DDRPHYCR    ;
    volatile Uint32 DDRPHYSR    ;
    volatile Uint8  RSVD7 [4]   ;
    volatile Uint32 VTPCTRL     ;
    volatile Uint32 VTPSTAT     ;
} DM6437_ddrRegs ;

/** ============================================================================
 *  @name   DM6437_pscRegs
 *
 *  @desc   Register Overlay Structure for PSC.
 *  ============================================================================
 */
typedef struct DM6437_pscRegs_tag {
    volatile Uint32 PID          ;
    volatile Uint8  RSVD0 [12]   ;
    volatile Uint32 GBLCTL       ;
    volatile Uint8  RSVD1 [4]    ;
    volatile Uint32 INTEVAL      ;
    volatile Uint8  RSVD2 [36]   ;
    volatile Uint32 ERRPR0       ;
    volatile Uint32 ERRPR1       ;
    volatile Uint8  RSVD3 [8]    ;
    volatile Uint32 ERRCR0       ;
    volatile Uint32 ERRCR1       ;
    volatile Uint8  RSVD4 [8]    ;
    volatile Uint32 PERRPR       ;
    volatile Uint8  RSVD5 [4]    ;
    volatile Uint32 PERRCR       ;
    volatile Uint8  RSVD6 [4]    ;
    volatile Uint32 EPCPR        ;
    volatile Uint8  RSVD7 [4]    ;
    volatile Uint32 EPCR         ;
    volatile Uint8  RSVD8 [132]  ;
    volatile Uint32 RAILSTAT     ;
    volatile Uint32 RAILCTL      ;
    volatile Uint32 RAILSEL      ;
    volatile Uint8  RSVD9 [20]   ;
    volatile Uint32 PTCMD        ;
    volatile Uint8  RSVD10 [4]   ;
    volatile Uint32 PTSTAT       ;
    volatile Uint8  RSVD11 [212] ;
    volatile Uint32 PDSTAT [41]  ;
    volatile Uint8  RSVD12 [92]  ;
    volatile Uint32 PDCTL [41]   ;
    volatile Uint8  RSVD13 [348] ;
    volatile Uint32 MRSTOUT [2]  ;
    volatile Uint8  RSVD14 [8]   ;
    volatile Uint32 MCKOUT [2]   ;
    volatile Uint8  RSVD15 [232] ;
    volatile Uint32 MDCFG [41]   ;
    volatile Uint8  RSVD16 [348] ;
    volatile Uint32 MDSTAT [41]  ;
    volatile Uint8  RSVD17 [348] ;
    volatile Uint32 MDCTL [41]   ;
} DM6437_pscRegs ;

/** ============================================================================
 *  @name   DM6437_paramEntry
 *
 *  @desc   Register Overlay Structure for PARAMENTRY.
 *  ============================================================================
 */
typedef struct DM6437_paramEntry_tags {
    volatile Uint32 OPTION        ;
    volatile Uint32 SRC           ;
    volatile Uint32 A_B_CNT       ;
    volatile Uint32 DST           ;
    volatile Uint32 SRC_DST_BIDX  ;
    volatile Uint32 LINK_BCNTRLD  ;
    volatile Uint32 SRC_DST_CIDX  ;
    volatile Uint32 CCNT          ;
} DM6437_paramEntry ;

/** ============================================================================
 *  @name   DM6437_edmaRegs
 *
 *  @desc   Register Overlay Structure for EDMA.
 *  ============================================================================
 */
typedef struct DM6437_edmaRegs_tag {
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
    DM6437_paramEntry PARAMENTRY[512]    ;
} DM6437_edmaRegs ;

/** ============================================================================
 *  @name   DM6437_pciRegs
 *
 *  @desc   PCI Back end register overlay structure.
 *  ============================================================================
 */
typedef struct DM6437_pciRegs_tag {
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
} DM6437_pciRegs ;


/** ============================================================================
 *  @name   DM6437_HalPciPhyObj_tag
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
 *  @field  barPhys
 *              Physical address of BAR areas .
 *  @field  barLen
 *              Length of BAR areas .
 *  @field  barVirt
 *              Virtual address of BAR areas .
 *  @field  chnlId
 *              EDMA Channel number.
 *  ============================================================================
 */
struct DM6437_HalPciPhyObj_tag {
    Pvoid          dev                ;
    Uint32         irqId              ;
    Uint32         busNo              ;
    Uint32         slotNo             ;
    Uint32         prevPageAddr       ;
    Uint32         barPhys [NUM_BARS] ;
    Uint32         barLen  [NUM_BARS] ;
    Uint32         barVirt [NUM_BARS] ;
    Uint32         edmaChnlId         ;
} ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM6437_HAL_PCI_H) */
