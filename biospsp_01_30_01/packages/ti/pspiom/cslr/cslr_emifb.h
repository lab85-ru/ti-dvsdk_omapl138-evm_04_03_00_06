/*
 * cslr_emifb.h
 *
 * This file contains the macros for Register Chip Support Library (CSL) which 
 * can be used for operations on the respective underlying hardware/peripheral
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

#ifndef _CSLR_EMIFB_H_
#define _CSLR_EMIFB_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <ti/pspiom/cslr/cslr.h>
#include <ti/pspiom/cslr/tistdtypes.h>


/* Minimum unit = 4 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 REVID;
    volatile Uint8 RSVD0[4];
    volatile Uint32 SDCFG;
    volatile Uint32 SDRFC;
    volatile Uint32 SDTIM1;
    volatile Uint32 SDTIM2;
    volatile Uint8 RSVD1[4];
    volatile Uint32 SDCFG2;
    volatile Uint32 BPRIO;
    volatile Uint8 RSVD2[28];
    volatile Uint32 PC1;
    volatile Uint32 PC2;
    volatile Uint32 PCC;
    volatile Uint32 PCMRS;
    volatile Uint32 PCT;
    volatile Uint8 RSVD3[12];
    volatile Uint32 DDRPHYRST;
    volatile Uint8 RSVD4[92];
    volatile Uint32 IRR;
    volatile Uint32 IMR;
    volatile Uint32 IMSR;
    volatile Uint32 IMCR;
} CSL_EmifbRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_EmifbRegs             *CSL_EmifbRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* REVID */

#define CSL_EMIFB_REVID_REV_MASK         (0xFFFFFFFFu)
#define CSL_EMIFB_REVID_REV_SHIFT        (0x00000000u)
#define CSL_EMIFB_REVID_REV_RESETVAL     (0x4033131Fu)

#define CSL_EMIFB_REVID_RESETVAL         (0x4033131Fu)

/* SDCFG */


#define CSL_EMIFB_SDCFG_MSDRAM_ENABLE_MASK (0x02000000u)
#define CSL_EMIFB_SDCFG_MSDRAM_ENABLE_SHIFT (0x00000019u)
#define CSL_EMIFB_SDCFG_MSDRAM_ENABLE_RESETVAL (0x00000000u)
/*----MSDRAM_ENABLE Tokens----*/
#define CSL_EMIFB_SDCFG_MSDRAM_ENABLE_MSDR_DIS (0x00000000u)
#define CSL_EMIFB_SDCFG_MSDRAM_ENABLE_MSDR_EN (0x00000001u)


#define CSL_EMIFB_SDCFG_BOOT_UNLOCK_MASK (0x00800000u)
#define CSL_EMIFB_SDCFG_BOOT_UNLOCK_SHIFT (0x00000017u)
#define CSL_EMIFB_SDCFG_BOOT_UNLOCK_RESETVAL (0x00000000u)
/*----BOOT_UNLOCK Tokens----*/
#define CSL_EMIFB_SDCFG_BOOT_UNLOCK_LOCKED (0x00000000u)
#define CSL_EMIFB_SDCFG_BOOT_UNLOCK_UNLOCKED (0x00000001u)


#define CSL_EMIFB_SDCFG_SDREN_MASK       (0x00010000u)
#define CSL_EMIFB_SDCFG_SDREN_SHIFT      (0x00000010u)
#define CSL_EMIFB_SDCFG_SDREN_RESETVAL   (0x00000001u)
/*----SDREN Tokens----*/
#define CSL_EMIFB_SDCFG_SDREN_SDR_DISABLE (0x00000000u)
#define CSL_EMIFB_SDCFG_SDREN_SDR_ENABLE (0x00000001u)

#define CSL_EMIFB_SDCFG_TIMUNLOCK_MASK   (0x00008000u)
#define CSL_EMIFB_SDCFG_TIMUNLOCK_SHIFT  (0x0000000Fu)
#define CSL_EMIFB_SDCFG_TIMUNLOCK_RESETVAL (0x00000000u)
/*----TIMUNLOCK Tokens----*/
#define CSL_EMIFB_SDCFG_TIMUNLOCK_CLEAR  (0x00000000u)
#define CSL_EMIFB_SDCFG_TIMUNLOCK_SET    (0x00000001u)

#define CSL_EMIFB_SDCFG_NM_MASK          (0x00004000u)
#define CSL_EMIFB_SDCFG_NM_SHIFT         (0x0000000Eu)
#define CSL_EMIFB_SDCFG_NM_RESETVAL      (0x00000000u)
/*----NM Tokens----*/
#define CSL_EMIFB_SDCFG_NM_32BIT         (0x00000000u)
#define CSL_EMIFB_SDCFG_NM_16BIT         (0x00000001u)


#define CSL_EMIFB_SDCFG_CL_MASK          (0x00000E00u)
#define CSL_EMIFB_SDCFG_CL_SHIFT         (0x00000009u)
#define CSL_EMIFB_SDCFG_CL_RESETVAL      (0x00000003u)
/*----CL Tokens----*/
#define CSL_EMIFB_SDCFG_CL_RSVD          (0x00000000u)
#define CSL_EMIFB_SDCFG_CL_RSVD1         (0x00000001u)
#define CSL_EMIFB_SDCFG_CL_TWO           (0x00000002u)
#define CSL_EMIFB_SDCFG_CL_THREE         (0x00000003u)


#define CSL_EMIFB_SDCFG_IBANK_MASK       (0x00000070u)
#define CSL_EMIFB_SDCFG_IBANK_SHIFT      (0x00000004u)
#define CSL_EMIFB_SDCFG_IBANK_RESETVAL   (0x00000002u)
/*----IBANK Tokens----*/
#define CSL_EMIFB_SDCFG_IBANK_ONE        (0x00000000u)
#define CSL_EMIFB_SDCFG_IBANK_TWO        (0x00000001u)
#define CSL_EMIFB_SDCFG_IBANK_FOUR       (0x00000002u)

#define CSL_EMIFB_SDCFG_EBANK_MASK       (0x00000008u)
#define CSL_EMIFB_SDCFG_EBANK_SHIFT      (0x00000003u)
#define CSL_EMIFB_SDCFG_EBANK_RESETVAL   (0x00000000u)
/*----EBANK Tokens----*/
#define CSL_EMIFB_SDCFG_EBANK_CS0        (0x00000000u)
#define CSL_EMIFB_SDCFG_EBANK_CS0_AND_CS1 (0x00000001u)

#define CSL_EMIFB_SDCFG_PAGESIZE_MASK    (0x00000007u)
#define CSL_EMIFB_SDCFG_PAGESIZE_SHIFT   (0x00000000u)
#define CSL_EMIFB_SDCFG_PAGESIZE_RESETVAL (0x00000000u)
/*----PAGESIZE Tokens----*/
#define CSL_EMIFB_SDCFG_PAGESIZE_256W_PAGE (0x00000000u)
#define CSL_EMIFB_SDCFG_PAGESIZE_512W_PAGE (0x00000001u)
#define CSL_EMIFB_SDCFG_PAGESIZE_1024W_PAGE (0x00000002u)
#define CSL_EMIFB_SDCFG_PAGESIZE_2048W_PAGE (0x00000003u)

#define CSL_EMIFB_SDCFG_RESETVAL         (0x00010620u)

/* SDRFC */

#define CSL_EMIFB_SDRFC_LP_MODE_MASK     (0x80000000u)
#define CSL_EMIFB_SDRFC_LP_MODE_SHIFT    (0x0000001Fu)
#define CSL_EMIFB_SDRFC_LP_MODE_RESETVAL (0x00000000u)
/*----LP_MODE Tokens----*/
#define CSL_EMIFB_SDRFC_LP_MODE_LPMODE   (0x00000000u)
#define CSL_EMIFB_SDRFC_LP_MODE_NO_LPMODE (0x00000001u)

#define CSL_EMIFB_SDRFC_MCLKSTOP_EN_MASK (0x40000000u)
#define CSL_EMIFB_SDRFC_MCLKSTOP_EN_SHIFT (0x0000001Eu)
#define CSL_EMIFB_SDRFC_MCLKSTOP_EN_RESETVAL (0x00000000u)
/*----MCLKSTOP_EN Tokens----*/
#define CSL_EMIFB_SDRFC_MCLKSTOP_EN_MCLKSTOP_DIS (0x00000000u)
#define CSL_EMIFB_SDRFC_MCLKSTOP_EN_MCLKSTOP_EN (0x00000001u)


#define CSL_EMIFB_SDRFC_SR_PD_MASK       (0x00800000u)
#define CSL_EMIFB_SDRFC_SR_PD_SHIFT      (0x00000017u)
#define CSL_EMIFB_SDRFC_SR_PD_RESETVAL   (0x00000000u)
/*----SR_PD Tokens----*/
#define CSL_EMIFB_SDRFC_SR_PD_SELF_REFRESH (0x00000000u)
#define CSL_EMIFB_SDRFC_SR_PD_POWER_DOWN (0x00000001u)


#define CSL_EMIFB_SDRFC_REFRESH_RATE_MASK (0x0000FFFFu)
#define CSL_EMIFB_SDRFC_REFRESH_RATE_SHIFT (0x00000000u)
#define CSL_EMIFB_SDRFC_REFRESH_RATE_RESETVAL (0x000004E2u)

#define CSL_EMIFB_SDRFC_RESETVAL         (0x000004E2u)

/* SDTIM1 */

#define CSL_EMIFB_SDTIM1_T_RFC_MASK      (0xFE000000u)
#define CSL_EMIFB_SDTIM1_T_RFC_SHIFT     (0x00000019u)
#define CSL_EMIFB_SDTIM1_T_RFC_RESETVAL  (0x0000000Au)

#define CSL_EMIFB_SDTIM1_T_RP_MASK       (0x01C00000u)
#define CSL_EMIFB_SDTIM1_T_RP_SHIFT      (0x00000016u)
#define CSL_EMIFB_SDTIM1_T_RP_RESETVAL   (0x00000003u)

#define CSL_EMIFB_SDTIM1_T_RCD_MASK      (0x00380000u)
#define CSL_EMIFB_SDTIM1_T_RCD_SHIFT     (0x00000013u)
#define CSL_EMIFB_SDTIM1_T_RCD_RESETVAL  (0x00000003u)

#define CSL_EMIFB_SDTIM1_T_WR_MASK       (0x00070000u)
#define CSL_EMIFB_SDTIM1_T_WR_SHIFT      (0x00000010u)
#define CSL_EMIFB_SDTIM1_T_WR_RESETVAL   (0x00000001u)

#define CSL_EMIFB_SDTIM1_T_RAS_MASK      (0x0000F800u)
#define CSL_EMIFB_SDTIM1_T_RAS_SHIFT     (0x0000000Bu)
#define CSL_EMIFB_SDTIM1_T_RAS_RESETVAL  (0x00000007u)

#define CSL_EMIFB_SDTIM1_T_RC_MASK       (0x000007C0u)
#define CSL_EMIFB_SDTIM1_T_RC_SHIFT      (0x00000006u)
#define CSL_EMIFB_SDTIM1_T_RC_RESETVAL   (0x0000000Au)

#define CSL_EMIFB_SDTIM1_T_RRD_MASK      (0x00000038u)
#define CSL_EMIFB_SDTIM1_T_RRD_SHIFT     (0x00000003u)
#define CSL_EMIFB_SDTIM1_T_RRD_RESETVAL  (0x00000002u)


#define CSL_EMIFB_SDTIM1_RESETVAL        (0x14D93A90u)

/* SDTIM2 */


#define CSL_EMIFB_SDTIM2_T_RAS_MAX_MASK  (0x78000000u)
#define CSL_EMIFB_SDTIM2_T_RAS_MAX_SHIFT (0x0000001Bu)
#define CSL_EMIFB_SDTIM2_T_RAS_MAX_RESETVAL (0x0000000Eu)


#define CSL_EMIFB_SDTIM2_T_XSR_MASK      (0x007F0000u)
#define CSL_EMIFB_SDTIM2_T_XSR_SHIFT     (0x00000010u)
#define CSL_EMIFB_SDTIM2_T_XSR_RESETVAL  (0x0000000Au)


#define CSL_EMIFB_SDTIM2_T_CKE_MASK      (0x0000001Fu)
#define CSL_EMIFB_SDTIM2_T_CKE_SHIFT     (0x00000000u)
#define CSL_EMIFB_SDTIM2_T_CKE_RESETVAL  (0x00000007u)

#define CSL_EMIFB_SDTIM2_RESETVAL        (0x700A0007u)

/* SDCFG2 */


#define CSL_EMIFB_SDCFG2_PASR_MASK       (0x00070000u)
#define CSL_EMIFB_SDCFG2_PASR_SHIFT      (0x00000010u)
#define CSL_EMIFB_SDCFG2_PASR_RESETVAL   (0x00000000u)
/*----PASR Tokens----*/
#define CSL_EMIFB_SDCFG2_PASR_FOUR_BANK  (0x00000000u)
#define CSL_EMIFB_SDCFG2_PASR_TWO_BANK   (0x00000001u)
#define CSL_EMIFB_SDCFG2_PASR_ONE_BANK   (0x00000002u)
#define CSL_EMIFB_SDCFG2_PASR_HALF_BANK  (0x00000005u)
#define CSL_EMIFB_SDCFG2_PASR_QRTR_BANK  (0x00000006u)
#define CSL_EMIFB_SDCFG2_PASR_RSVD1      (0x00000007u)


#define CSL_EMIFB_SDCFG2_ROWSIZE_MASK    (0x00000007u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_SHIFT   (0x00000000u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_RESETVAL (0x00000000u)
/*----ROWSIZE Tokens----*/
#define CSL_EMIFB_SDCFG2_ROWSIZE_9BITS   (0x00000000u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_10BITS  (0x00000001u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_11BITS  (0x00000002u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_12BITS  (0x00000003u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_13BITS  (0x00000004u)
#define CSL_EMIFB_SDCFG2_ROWSIZE_14BITS  (0x00000005u)

#define CSL_EMIFB_SDCFG2_RESETVAL        (0x00000000u)

/* BPRIO */


#define CSL_EMIFB_BPRIO_PRIO_RAISE_MASK  (0x000000FFu)
#define CSL_EMIFB_BPRIO_PRIO_RAISE_SHIFT (0x00000000u)
#define CSL_EMIFB_BPRIO_PRIO_RAISE_RESETVAL (0x000000FFu)

#define CSL_EMIFB_BPRIO_RESETVAL         (0x000000FFu)

/* PC1 */

#define CSL_EMIFB_PC1_COUNTER1_MASK      (0xFFFFFFFFu)
#define CSL_EMIFB_PC1_COUNTER1_SHIFT     (0x00000000u)
#define CSL_EMIFB_PC1_COUNTER1_RESETVAL  (0x00000000u)

#define CSL_EMIFB_PC1_RESETVAL           (0x00000000u)

/* PC2 */

#define CSL_EMIFB_PC2_COUNTER2_MASK      (0xFFFFFFFFu)
#define CSL_EMIFB_PC2_COUNTER2_SHIFT     (0x00000000u)
#define CSL_EMIFB_PC2_COUNTER2_RESETVAL  (0x00000000u)

#define CSL_EMIFB_PC2_RESETVAL           (0x00000000u)

/* PCC */

#define CSL_EMIFB_PCC_CNTR2_MSTID_EN_MASK (0x80000000u)
#define CSL_EMIFB_PCC_CNTR2_MSTID_EN_SHIFT (0x0000001Fu)
#define CSL_EMIFB_PCC_CNTR2_MSTID_EN_RESETVAL (0x00000000u)

#define CSL_EMIFB_PCC_CNTR2_REGION_EN_MASK (0x40000000u)
#define CSL_EMIFB_PCC_CNTR2_REGION_EN_SHIFT (0x0000001Eu)
#define CSL_EMIFB_PCC_CNTR2_REGION_EN_RESETVAL (0x00000000u)


#define CSL_EMIFB_PCC_CNTR2_CFG_MASK     (0x000F0000u)
#define CSL_EMIFB_PCC_CNTR2_CFG_SHIFT    (0x00000010u)
#define CSL_EMIFB_PCC_CNTR2_CFG_RESETVAL (0x00000000u)

#define CSL_EMIFB_PCC_CNTR1_MSTID_EN_MASK (0x00008000u)
#define CSL_EMIFB_PCC_CNTR1_MSTID_EN_SHIFT (0x0000000Fu)
#define CSL_EMIFB_PCC_CNTR1_MSTID_EN_RESETVAL (0x00000000u)

#define CSL_EMIFB_PCC_CNTR1_REGION_EN_MASK (0x00004000u)
#define CSL_EMIFB_PCC_CNTR1_REGION_EN_SHIFT (0x0000000Eu)
#define CSL_EMIFB_PCC_CNTR1_REGION_EN_RESETVAL (0x00000000u)


#define CSL_EMIFB_PCC_CNTR1_CFG_MASK     (0x0000000Fu)
#define CSL_EMIFB_PCC_CNTR1_CFG_SHIFT    (0x00000000u)
#define CSL_EMIFB_PCC_CNTR1_CFG_RESETVAL (0x00000000u)

#define CSL_EMIFB_PCC_RESETVAL           (0x00000000u)

/* PCMRS */

#define CSL_EMIFB_PCMRS_MST_ID2_MASK     (0xFF000000u)
#define CSL_EMIFB_PCMRS_MST_ID2_SHIFT    (0x00000018u)
#define CSL_EMIFB_PCMRS_MST_ID2_RESETVAL (0x00000000u)


#define CSL_EMIFB_PCMRS_MST_ID1_MASK     (0x0000FF00u)
#define CSL_EMIFB_PCMRS_MST_ID1_SHIFT    (0x00000008u)
#define CSL_EMIFB_PCMRS_MST_ID1_RESETVAL (0x00000000u)


#define CSL_EMIFB_PCMRS_RESETVAL         (0x00000000u)

/* PCT */

#define CSL_EMIFB_PCT_TOTAL_TIME_MASK    (0xFFFFFFFFu)
#define CSL_EMIFB_PCT_TOTAL_TIME_SHIFT   (0x00000000u)
#define CSL_EMIFB_PCT_TOTAL_TIME_RESETVAL (0x00000000u)

#define CSL_EMIFB_PCT_RESETVAL           (0x00000000u)

/* DDRPHYRST */


#define CSL_EMIFB_DDRPHYRST_RESETPHY_MASK (0x00000400u)
#define CSL_EMIFB_DDRPHYRST_RESETPHY_SHIFT (0x0000000Au)
#define CSL_EMIFB_DDRPHYRST_RESETPHY_RESETVAL (0x00000000u)
/*----RESETPHY Tokens----*/
#define CSL_EMIFB_DDRPHYRST_RESETPHY_RESET (0x00000001u)


#define CSL_EMIFB_DDRPHYRST_RESETVAL     (0x00002091u)

/* IRR */


#define CSL_EMIFB_IRR_LT_MASK            (0x00000004u)
#define CSL_EMIFB_IRR_LT_SHIFT           (0x00000002u)
#define CSL_EMIFB_IRR_LT_RESETVAL        (0x00000000u)
/*----LT Tokens----*/
#define CSL_EMIFB_IRR_LT_NOT_OCCURRED    (0x00000000u)
#define CSL_EMIFB_IRR_LT_OCCURRED        (0x00000001u)
#define CSL_EMIFB_IRR_LT_CLEAR           (0x00000001u)


#define CSL_EMIFB_IRR_RESETVAL           (0x00000000u)

/* IMR */


#define CSL_EMIFB_IMR_LTM_MASK           (0x00000004u)
#define CSL_EMIFB_IMR_LTM_SHIFT          (0x00000002u)
#define CSL_EMIFB_IMR_LTM_RESETVAL       (0x00000000u)
/*----LTM Tokens----*/
#define CSL_EMIFB_IMR_LTM_NOT_OCCURRED   (0x00000000u)
#define CSL_EMIFB_IMR_LTM_OCCURRED       (0x00000001u)
#define CSL_EMIFB_IMR_LTM_CLEAR          (0x00000001u)


#define CSL_EMIFB_IMR_RESETVAL           (0x00000000u)

/* IMSR */


#define CSL_EMIFB_IMSR_LTMSET_MASK       (0x00000004u)
#define CSL_EMIFB_IMSR_LTMSET_SHIFT      (0x00000002u)
#define CSL_EMIFB_IMSR_LTMSET_RESETVAL   (0x00000000u)
/*----LTMSET Tokens----*/
#define CSL_EMIFB_IMSR_LTMSET_DISABLED   (0x00000000u)
#define CSL_EMIFB_IMSR_LTMSET_ENABLED    (0x00000001u)
#define CSL_EMIFB_IMSR_LTMSET_SET        (0x00000001u)


#define CSL_EMIFB_IMSR_RESETVAL          (0x00000000u)

/* IMCR */


#define CSL_EMIFB_IMCR_LTMCLR_MASK       (0x00000004u)
#define CSL_EMIFB_IMCR_LTMCLR_SHIFT      (0x00000002u)
#define CSL_EMIFB_IMCR_LTMCLR_RESETVAL   (0x00000000u)
/*----LTMCLR Tokens----*/
#define CSL_EMIFB_IMCR_LTMCLR_DISABLED   (0x00000000u)
#define CSL_EMIFB_IMCR_LTMCLR_ENABLED    (0x00000001u)
#define CSL_EMIFB_IMCR_LTMCLR_CLEAR      (0x00000001u)


#define CSL_EMIFB_IMCR_RESETVAL          (0x00000000u)

#ifdef __cplusplus
}
#endif


#endif
