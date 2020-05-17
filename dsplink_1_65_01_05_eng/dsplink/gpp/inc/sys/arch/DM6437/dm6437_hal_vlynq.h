/** ============================================================================
 *  @file   dm6437_hal_vlynq.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/DM6437/
 *
 *  @desc   Hardware Abstraction Layer for PC-DM64LC VLYNQ interface.
 *          Defines interfaces to initialize the VLYNQ interface.
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


#if !defined (DM6437_HAL_VLYNQ_H)
#define DM6437_HAL_VLYNQ_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* if defined (__cplusplus) */


/** ============================================================================
 *  @const  VLYNQ_DMA_MAXTHROUGHPUT
 *
 *  @desc   Approx. Maximum through put of VLYNQ interface
 *          (100Mhz * 4 /32) words (32bit)/Sec.
 *  ============================================================================
 */
#define VLYNQ_DMA_MAXTHROUGHPUT     10u

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
 *  @const   LPSC_DDR
 *
 *  @desc    Module number for DDR.
 *  ============================================================================
 */
#define LPSC_DDR                    13u

/** ============================================================================
 *  @const   DM6437_DEVREG_BASE
 *
 *  @desc    Base address of Device config registers.
 *  ============================================================================
 */
#define DM6437_DEVREG_BASE          0x00440000u

/** ============================================================================
 *  @const   DM6437_PLLREG_BASE
 *
 *  @desc    Base address of PLL0 registers.
 *  ============================================================================
 */
#define DM6437_PLL0REG_BASE         0x00440800u

/** ============================================================================
 *  @const   DM6437_PLLREG_BASE
 *
 *  @desc    Base address of PLL1 registers.
 *  ============================================================================
 */
#define DM6437_PLL1REG_BASE         0x00440C00u

/** ============================================================================
 *  @const   DM6437_EDMAREG_BASE
 *
 *  @desc    Base address of EDMA registers.
 *  ============================================================================
 */
#define DM6437_EDMAREG_BASE         0x400000u

/** ============================================================================
 *  @const   DM6437_DDRREG_BASE
 *
 *  @desc    Base address of DDR PHY registers.
 *  ============================================================================
 */
#define DM6437_DDRREG_BASE          0x20000000u

/** ============================================================================
 *  @const   DM6437_PSCREG_BASE
 *
 *  @desc    Base address of PSC registers.
 *  ============================================================================
 */
#define DM6437_PSCREG_BASE          0x00441000u

/** ============================================================================
 *  @const   DM6437_PCIREG_BASE
 *
 *  @desc    Base address of PCI backend registers.
 *  ============================================================================
 */
#define DM6437_PCIREG_BASE          0x0041A000u

/** ============================================================================
 *  @const   DM6437_PEERVLYNQREG_BASE
 *
 *  @desc    Base address of VLYNQ registers.
 *  ============================================================================
 */
#define DM6437_PEERVLYNQREG_BASE    0x00601000u

/** ============================================================================
 *  @const   DM6437_SOFTINT1_MASK
 *
 *  @desc    Mask for generating soft int1 (DSP->GPP)
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
 *  @const   DM6437_VLYNQ_WINDOWSIZE
 *
 *  @desc    Total size exposed by VLYNQ 64MB.
 *  ============================================================================
 */
#define DM6437_VLYNQ_WINDOWSIZE     0x04000000u

/** ============================================================================
 *  @const   DRA44XGEM_EDMA_REGBASE
 *
 *  @desc    Base Address of DRA44xGEM EDMA controller registers.
 *  ============================================================================
 */
#define DRA44XGEM_EDMA_REGBASE      0x01C00000u


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
 *  @name   DM6437_pciRegs
 *
 *  @desc   PCI Back end register overlay structure.
 *  ============================================================================
 */
typedef struct DM6437_pciRegs {
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
 *  @name   DM6437_vlynqRegs
 *
 *  @desc   VLYNQ register structure.
 *  ============================================================================
 */
typedef struct DM6437_vlynqRegs {
    volatile Uint32 LOCAL_REVID       ;
    volatile Uint32 LOCAL_CTRL        ;
    volatile Uint32 LOCAL_STAT        ;
    volatile Uint32 LOCAL_INTPRI      ;
    volatile Uint32 LOCAL_INTSTATCLR  ;
    volatile Uint32 LOCAL_INTPENDSET  ;
    volatile Uint32 LOCAL_INTPTR      ;
    volatile Uint32 LOCAL_XAM         ;
    volatile Uint32 LOCAL_RAMS1       ;
    volatile Uint32 LOCAL_RAMO1       ;
    volatile Uint32 LOCAL_RAMS2       ;
    volatile Uint32 LOCAL_RAMO2       ;
    volatile Uint32 LOCAL_RAMS3       ;
    volatile Uint32 LOCAL_RAMO3       ;
    volatile Uint32 LOCAL_RAMS4       ;
    volatile Uint32 LOCAL_RAMO4       ;
    volatile Uint32 LOCAL_CHIPVER     ;
    volatile Uint32 LOCAL_AUTNGO      ;
    volatile Uint32 reserved [14]     ;
    volatile Uint32 PEER_REVID        ;
    volatile Uint32 PEER_CTRL         ;
    volatile Uint32 PEER_STAT         ;
    volatile Uint32 PEER_INTPRI       ;
    volatile Uint32 PEER_INTSTATCLR   ;
    volatile Uint32 PEER_INTPENDSET   ;
    volatile Uint32 PEER_INTPTR       ;
    volatile Uint32 PEER_XAM          ;
    volatile Uint32 PEER_RAMS1        ;
    volatile Uint32 PEER_RAMO1        ;
    volatile Uint32 PEER_RAMS2        ;
    volatile Uint32 PEER_RAMO2        ;
    volatile Uint32 PEER_RAMS3        ;
    volatile Uint32 PEER_RAMO3        ;
    volatile Uint32 PEER_RAMS4        ;
    volatile Uint32 PEER_RAMO4        ;
    volatile Uint32 PEER_CHIPVER      ;
    volatile Uint32 PEER_AUTNGO       ;
    volatile Uint32 PEER_MANNGO       ;
    volatile Uint32 PEER_NGOSTAT      ;
    volatile Uint32 PEER_INTVEC0      ;
    volatile Uint32 PEER_INTVEC1      ;
} DM6437_vlynqRegs ;

/** ============================================================================
 *  @name   DRA44XGEM_paramEntry
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
} DRA44XGEM_paramEntry ;

/** ============================================================================
 *  @name   DRA44XGEM_EDMA3_CCRL_DraRegs
 *
 *  @desc   Register Overlay Structure for DRA.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 DRAE;
    volatile Uint32 DRAEH;
} DRA44XGEM_EDMA3_CCRL_DraRegs;

/** ============================================================================
 *  @name   DRA44XGEM_EDMA3_CCRL_QueevtentryRegs
 *
 *  @desc   Register Overlay Structure for QUEEVTENTRY.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 QUEEVT_ENTRY;
} DRA44XGEM_EDMA3_CCRL_QueevtentryRegs;

/** ============================================================================
 *  @name   DRA44XGEM_EDMA3_CCRL_ShadowRegs
 *
 *  @desc   Register Overlay Structure for SHADOW.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 ER;
    volatile Uint32 ERH;
    volatile Uint32 ECR;
    volatile Uint32 ECRH;
    volatile Uint32 ESR;
    volatile Uint32 ESRH;
    volatile Uint32 CER;
    volatile Uint32 CERH;
    volatile Uint32 EER;
    volatile Uint32 EERH;
    volatile Uint32 EECR;
    volatile Uint32 EECRH;
    volatile Uint32 EESR;
    volatile Uint32 EESRH;
    volatile Uint32 SER;
    volatile Uint32 SERH;
    volatile Uint32 SECR;
    volatile Uint32 SECRH;
    volatile Uint8  RSVD0[8];
    volatile Uint32 IER;
    volatile Uint32 IERH;
    volatile Uint32 IECR;
    volatile Uint32 IECRH;
    volatile Uint32 IESR;
    volatile Uint32 IESRH;
    volatile Uint32 IPR;
    volatile Uint32 IPRH;
    volatile Uint32 ICR;
    volatile Uint32 ICRH;
    volatile Uint32 IEVAL;
    volatile Uint8  RSVD1[4];
    volatile Uint32 QER;
    volatile Uint32 QEER;
    volatile Uint32 QEECR;
    volatile Uint32 QEESR;
    volatile Uint32 QSER;
    volatile Uint32 QSECR;
    volatile Uint8  RSVD2[360];
} DRA44XGEM_EDMA3_CCRL_ShadowRegs;

/** ============================================================================
 *  @name   DRA44XGEM_edmaRegs
 *
 *  @desc   Register Overlay Structure for EDMA.
 *  ============================================================================
 */
typedef struct  {
    volatile Uint32 REV;
    volatile Uint32 CCCFG;
    volatile Uint8  RSVD0[248];
    volatile Uint32 DCHMAP[64];
    volatile Uint32 QCHMAP[8];
    volatile Uint8  RSVD1[32];
    volatile Uint32 DMAQNUM[8];
    volatile Uint32 QDMAQNUM;
    volatile Uint8  RSVD2[28];
    volatile Uint32 QUETCMAP;
    volatile Uint32 QUEPRI;
    volatile Uint8  RSVD3[120];
    volatile Uint32 EMR;
    volatile Uint32 EMRH;
    volatile Uint32 EMCR;
    volatile Uint32 EMCRH;
    volatile Uint32 QEMR;
    volatile Uint32 QEMCR;
    volatile Uint32 CCERR;
    volatile Uint32 CCERRCLR;
    volatile Uint32 EEVAL;
    volatile Uint8  RSVD4[28];
    DRA44XGEM_EDMA3_CCRL_DraRegs DRA[8];
    volatile Uint32 QRAE[8];
    volatile Uint8  RSVD5[96];
    DRA44XGEM_EDMA3_CCRL_QueevtentryRegs QUEEVTENTRY[8][16];
    volatile Uint32 QSTAT[8];
    volatile Uint32 QWMTHRA;
    volatile Uint32 QWMTHRB;
    volatile Uint8  RSVD6[24];
    volatile Uint32 CCSTAT;
    volatile Uint8  RSVD7[188];
    volatile Uint32 AETCTL;
    volatile Uint32 AETSTAT;
    volatile Uint32 AETCMD;
    volatile Uint8  RSVD8[244];
    volatile Uint32 MPFAR;
    volatile Uint32 MPFSR;
    volatile Uint32 MPFCR;
    volatile Uint32 MPPAG;
    volatile Uint32 MPPA[8];
    volatile Uint8  RSVD9[2000];
    volatile Uint32 ER;
    volatile Uint32 ERH;
    volatile Uint32 ECR;
    volatile Uint32 ECRH;
    volatile Uint32 ESR;
    volatile Uint32 ESRH;
    volatile Uint32 CER;
    volatile Uint32 CERH;
    volatile Uint32 EER;
    volatile Uint32 EERH;
    volatile Uint32 EECR;
    volatile Uint32 EECRH;
    volatile Uint32 EESR;
    volatile Uint32 EESRH;
    volatile Uint32 SER;
    volatile Uint32 SERH;
    volatile Uint32 SECR;
    volatile Uint32 SECRH;
    volatile Uint8  RSVD10[8];
    volatile Uint32 IER;
    volatile Uint32 IERH;
    volatile Uint32 IECR;
    volatile Uint32 IECRH;
    volatile Uint32 IESR;
    volatile Uint32 IESRH;
    volatile Uint32 IPR;
    volatile Uint32 IPRH;
    volatile Uint32 ICR;
    volatile Uint32 ICRH;
    volatile Uint32 IEVAL;
    volatile Uint8  RSVD11[4];
    volatile Uint32 QER;
    volatile Uint32 QEER;
    volatile Uint32 QEECR;
    volatile Uint32 QEESR;
    volatile Uint32 QSER;
    volatile Uint32 QSECR;
    volatile Uint8  RSVD12[3944];
    DRA44XGEM_EDMA3_CCRL_ShadowRegs SHADOW[8];
    volatile Uint8 RSVD13[4096];
    DRA44XGEM_paramEntry PARAMENTRY[512];
} DRA44XGEM_edmaRegs;

/** ============================================================================
 *  @name   DM6437_HalVlynqPhyObj_tag
 *
 *  @desc   Physical Interface object.
 *
 *  @field  region1Addr
 *              Region 1 Base Address .
 *  @field  region1Size
 *              Region 1 Size .
 *  @field  region2Addr
 *              Region 2 Base Address .
 *  @field  region2Size
 *              Region 2 Size .
 *  @field  region3Addr
 *              Region 3 Base Address .
 *  @field  region3Size
 *              Region 3 Size .
 *  @field  region4Addr
 *              Region 4 Base Address .
 *  @field  region4Size
 *              Region 4 Size .
 *  @field  prevUserAddr
 *              Stores previous page base address .
 *  @field  prevUserSize
 *              Stores previous page size .
 *  @field  ctrlBaseAddr
 *              Base Address of VLYNQ Control.
 *  @field  intVector
 *              Vlynq interrupt vector number.
 *  @field  edmaChnlId
 *              Edma channel number.
 *  ============================================================================
 */
struct DM6437_HalVlynqPhyObj_tag {
    Uint32 region1Addr   ;
    Uint32 region1Size   ;
    Uint32 region2Addr   ;
    Uint32 region2Size   ;
    Uint32 region3Addr   ;
    Uint32 region3Size   ;
    Uint32 region4Addr   ;
    Uint32 region4Size   ;
    Uint32 prevUserAddr  ;
    Uint32 prevUserSize  ;
    Uint32 ctrlBaseAddr  ;
    Uint8  intVector     ;
    Uint32 edmaChnlId    ;
} ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM6437_HAL_VLYNQ_H) */
