/** ============================================================================
 *  @file   _signature.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines the file and object signatures used in DSP/BIOS Link
 *          These signatures are used in object validation and error reporting.
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


#if !defined (_SIGNATURE_H)
#define _SIGNATURE_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <gpptypes.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const  File identifiers
 *
 *  @desc   File identifiers used in TRC_SetReason ()
 *  ============================================================================
 */
/*  ============================================================================
 *  API File identifiers
 *  ============================================================================
 */
#define FID_BASE_API            0x0200u
#define FID_C_API_PROC          (FID_BASE_API  + 0x0u)
#define FID_C_API_CHNL          (FID_BASE_API  + 0x1u)
#define FID_C_API_MSGQ          (FID_BASE_API  + 0x2u)
#define FID_C_API_POOL          (FID_BASE_API  + 0x3u)
#define FID_C_API_NOTIFY        (FID_BASE_API  + 0x4u)
#define FID_C_API_MPCS          (FID_BASE_API  + 0x5u)
#define FID_C_API_RINGIO        (FID_BASE_API  + 0x6u)
#define FID_C_API_SYNC          (FID_BASE_API  + 0x7u)
#define FID_C_API_MPLIST        (FID_BASE_API  + 0x8u)
#define FID_C_API_IDM           (FID_BASE_API  + 0x9u)
#define FID_C_API_MEM           (FID_BASE_API  + 0xAu)
#define FID_C_API_IPOOL         (FID_BASE_API  + 0xBu)
#define FID_C_API_MPCS_OS       (FID_BASE_API  + 0xCu)

/*  ============================================================================
 *  PMGR File identifiers
 *  ============================================================================
 */
#define FID_BASE_PMGR           0x0300u
#define FID_C_PMGR_PROC         (FID_BASE_PMGR + 0u)
#define FID_C_PMGR_CHNL         (FID_BASE_PMGR + 1u)
#define FID_C_PMGR_MSGQ         (FID_BASE_PMGR + 2u)

/*  ============================================================================
 *  GEN File identifiers
 *  ============================================================================
 */
#define FID_BASE_GEN            0x0400u
#define FID_C_GEN_UTILS         (FID_BASE_GEN  + 0u)
#define FID_C_GEN_LIST          (FID_BASE_GEN  + 1u)
#define FID_C_GEN_COFF          (FID_BASE_GEN  + 2u)
#define FID_C_GEN_COFF_INT      (FID_BASE_GEN  + 3u)
#define FID_C_GEN_COFF_55x      (FID_BASE_GEN  + 4u)
#define FID_C_GEN_COFF_64x      (FID_BASE_GEN  + 5u)
#define FID_C_GEN_STATICLOADER  (FID_BASE_GEN  + 6u)
#define FID_C_GEN_BINLOADER     (FID_BASE_GEN  + 7u)
#define FID_C_GEN_IDM           (FID_BASE_GEN  + 8u)
#define FID_C_GEN_NOLOADER      (FID_BASE_GEN  + 9u)
#define FID_C_GEN_COFF_MEM      (FID_BASE_GEN  + 0xAu)
#define FID_C_GEN_COFF_FILE     (FID_BASE_GEN  + 0xBu)
#define FID_C_GEN_COFF_SHM      (FID_BASE_GEN  + 0xCu)

/*  ============================================================================
 *  OSAL File identifiers
 *  ============================================================================
 */
#define FID_BASE_OSAL           0x0500u
#define FID_C_OSAL              (FID_BASE_OSAL + 0x0u)
#define FID_C_OSAL_DPC          (FID_BASE_OSAL + 0x1u)
#define FID_C_OSAL_ISR          (FID_BASE_OSAL + 0x2u)
#define FID_C_OSAL_KFILE        (FID_BASE_OSAL + 0x3u)
#define FID_C_OSAL_MEM          (FID_BASE_OSAL + 0x4u)
#define FID_C_OSAL_PRCS         (FID_BASE_OSAL + 0x5u)
#define FID_C_OSAL_SYNC         (FID_BASE_OSAL + 0x6u)
#define FID_C_OSAL_TRC          (FID_BASE_OSAL + 0x7u)
#define FID_C_OSAL_CFG          (FID_BASE_OSAL + 0x8u)
#define FID_C_OSAL_PRINT        (FID_BASE_OSAL + 0x9u)
#define FID_C_OSAL_DRV_PMGR     (FID_BASE_OSAL + 0xAu)
#define FID_C_OSAL_DRV_POOL     (FID_BASE_OSAL + 0xBu)
#define FID_C_OSAL_DRV_API      (FID_BASE_OSAL + 0xCu)
#define FID_C_OSAL_NOTIFY_KNL   (FID_BASE_OSAL + 0xDu)
#define FID_C_OSAL_USER         (FID_BASE_OSAL + 0xEu)
#define FID_C_OSAL_KFILE_PSEUDO (FID_BASE_OSAL + 0xFu)
#define FID_C_OSAL_CLK          (FID_BASE_OSAL + 0x10u)
#define FID_C_OSAL_REG          (FID_BASE_OSAL + 0x1u)

/*  ============================================================================
 *  LDRV File identifiers
 *  ============================================================================
 */
#define FID_BASE_LDRV           0x0700u
#define FID_C_LDRV              (FID_BASE_LDRV + 0x0u)
#define FID_C_LDRV_PROC         (FID_BASE_LDRV + 0x1u)
#define FID_C_LDRV_CHNL         (FID_BASE_LDRV + 0x2u)
#define FID_C_LDRV_MSGQ         (FID_BASE_LDRV + 0x3u)
#define FID_C_LDRV_CHIRPS       (FID_BASE_LDRV + 0x4u)
#define FID_C_LDRV_OS           (FID_BASE_LDRV + 0x5u)

/* LDRV DATA */
#define FID_C_LDRV_DATA         (FID_BASE_LDRV + 0x6u)
#define FID_C_LDRV_DATA_PCPY    (FID_BASE_LDRV + 0x7u)
#define FID_C_LDRV_DATA_ZCPY    (FID_BASE_LDRV + 0x8u)

/* LDRV DRV */
#define FID_C_LDRV_DRV          (FID_BASE_LDRV + 0x9u)
#define FID_C_LDRV_DRV_SHM      (FID_BASE_LDRV + 0xAu)

/* LDRV IPS */
#define FID_C_LDRV_IPS          (FID_BASE_LDRV + 0x16u)
#define FID_C_LDRV_IPS_IPS      (FID_BASE_LDRV + 0x17u)

/* LDRV MQT */
#define FID_C_LDRV_MQT          (FID_BASE_LDRV + 0x18u)
#define FID_C_LDRV_MQT_PCPY     (FID_BASE_LDRV + 0x19u)
#define FID_C_LDRV_MQT_ZCPY     (FID_BASE_LDRV + 0x1Au)

/* LDRV POOL */
#define FID_C_LDRV_POOL         (FID_BASE_LDRV + 0x1Bu)
#define FID_C_LDRV_POOL_BUF     (FID_BASE_LDRV + 0x1Cu)
#define FID_C_LDRV_POOL_DMA     (FID_BASE_LDRV + 0x1Du)
#define FID_C_LDRV_POOL_SMA     (FID_BASE_LDRV + 0x1Eu)

/* LDRV RINGIO */
#define FID_C_LDRV_RINGIO       (FID_BASE_LDRV + 0x1Fu)

/* LDRV MPLIST */
#define FID_C_LDRV_MPLIST       (FID_BASE_LDRV + 0x20u)

/* LDRV MPLIST */
#define FID_C_LDRV_MPCS         (FID_BASE_LDRV + 0x21u)

/* LDRV SMM */
#define FID_C_LDRV_SMM          (FID_BASE_LDRV + 0x22u)

/* LDRV MPLIST */
#define FID_C_LDRV_MPCS_OS      (FID_BASE_LDRV + 0x23u)

/*  ============================================================================
 *  ARCH File identifiers
 *  ============================================================================
 */
#define FID_BASE_ARCH           0x0800u
/* DSP */
#define FID_C_ARCH_DSP          (FID_BASE_ARCH + 0x00u)
/* DSP Interface */
#define FID_C_ARCH_DSP_INTF     (FID_BASE_ARCH + 0x01u)
/* CFGMAP */
#define FID_C_ARCH_CFG          (FID_BASE_ARCH + 0x02u)
/* HAL */
#define FID_C_ARCH_HAL          (FID_BASE_ARCH + 0x03u)
/* MAP */
#define FID_C_ARCH_HAL_MAP      (FID_BASE_ARCH + 0x04u)
/* Intgen */
#define FID_C_ARCH_HAL_INTGEN   (FID_BASE_ARCH + 0x05u)
/* DMA */
#define FID_C_ARCH_HAL_DMA      (FID_BASE_ARCH + 0x06u)
/* RDWR */
#define FID_C_ARCH_HAL_RDWR     (FID_BASE_ARCH + 0x07u)
/* BOOT */
#define FID_C_ARCH_HAL_BOOT     (FID_BASE_ARCH + 0x08u)
/* PWR */
#define FID_C_ARCH_HAL_PWR      (FID_BASE_ARCH + 0x09u)
/* MMU */
#define FID_C_ARCH_HAL_MMU      (FID_BASE_ARCH + 0x0Au)
/* PHY SHMEM */
#define FID_C_ARCH_PHY_SHMEM    (FID_BASE_ARCH + 0x0Bu)
/* PHY PCI */
#define FID_C_ARCH_PHY_PCI      (FID_BASE_ARCH + 0x0Cu)
/* PHY PCI */
#define FID_C_ARCH_PHY_VLYNQ    (FID_BASE_ARCH + 0x0Du)
/* DSP CLK */
#define FID_C_ARCH_DSPCLK       (FID_BASE_ARCH + 0x0Eu)



/*  ============================================================================
 *  @const  Object signatures
 *
 *  @desc   Object signatures used to validate objects.
 *  ============================================================================
 */
#define SIGN_NULL                0x00000000u      /* NULL signature */

/*  Processor Manager            hex value       String (in reverse) */
#define SIGN_PROC                0x434F5250u      /* PROC */
#define SIGN_CHNL                0x4C4E4843u      /* CHNL */
#define SIGN_CODE                0x45444F43u      /* CODE */
#define SIGN_PROC_SETUP          0x50535450u      /* PSTP */

/*  Link Driver                  hex value       String (in reverse) */
#define SIGN_DSP                 0x5F505344u      /* DSP_ */
#define SIGN_LDRV_DATA           0x5341444Cu      /* LDAT */
#define SIGN_LDRV_ISR            0x5253494Cu      /* LISR */
#define SIGN_LDRV_PROC           0x4352504Cu      /* LPRC */
#define SIGN_LDRV_CHNL           0x484E434Cu      /* LCHN */

/*  OS Adaptation Layer          hex value       String (in reverse) */
#define SIGN_CFG                 0x5F474643u      /* CFG_ */
#define SIGN_TRC                 0x5F435254u      /* TRC_ */
#define SIGN_DPC                 0x5F435044u      /* DPC_ */
#define SIGN_ISR                 0x5F525349u      /* ISR_ */
#define SIGN_KFILE               0x4C49464Bu      /* KFIL */
#define SIGN_MEM                 0x5F504550u      /* MEM_ */
#define SIGN_PRCS                0x53435250u      /* PRCS */
#define SIGN_SYNC                0x434E5953u      /* SYNC */
#define SIGN_DRV                 0x5F4B5244u      /* DRV_ */

/*  Generic components           hex value       String (in reverse) */
#define SIGN_GEN                 0x5F4E4547u      /* GEN_ */
#define SIGN_LIST                0x5453494Cu      /* LIST */

/* HAL                           hex value       String (in reverse) */
#define SIGN_HAL                 0x5F4C414Eu      /* HAL_ */


/*  ============================================================================
 *  @const  MAXIMUM_COMPONENTS
 *
 *  @desc   Maximum number of components
 *  ============================================================================
 */
#define MAXIMUM_COMPONENTS         16u

/*  ============================================================================
 *  @const  Component IDs
 *
 *  @desc   Component Identifiers. These must match corresponding bit
 *          position in component map
 *  ============================================================================
 */
#define ID_API_BASE             0x00010000u
#define ID_PMGR_BASE            0x00020000u
#define ID_GEN_BASE             0x00040000u
#define ID_OSAL_BASE            0x00080000u
#define ID_LDRV_BASE            0x00100000u
#define ID_LDRV_DATA_BASE       0x00200000u
#define ID_LDRV_DRV_BASE        0x00400000u
#define ID_LDRV_IPS_BASE        0x00800000u
#define ID_LDRV_MQT_BASE        0x01000000u
#define ID_LDRV_POOL_BASE       0x02000000u
#define ID_ARCH_BASE            0x04000000u
#define ID_ARCH_HAL_BASE        0x08000000u
#define ID_ARCH_PHY_BASE        0x10000000u

#define MIN_COMPONENT  ID_API_BASE          >> 16u
#define MAX_COMPONENT  ID_ARCH_PHY_BASE     >> 16u

/*  ============================================================================
 *  API Subcomponent map
 *  ============================================================================
 */
#define ID_API_PROC        ID_API_BASE  | 0x0001u
#define ID_API_CHNL        ID_API_BASE  | 0x0002u
#define ID_API_MSGQ        ID_API_BASE  | 0x0004u
#define ID_API_POOL        ID_API_BASE  | 0x0008u
#define ID_API_NOTIFY      ID_API_BASE  | 0x0010u
#define ID_API_MPCS        ID_API_BASE  | 0x0020u
#define ID_API_RINGIO      ID_API_BASE  | 0x0040u
#define ID_API_SYNC        ID_API_BASE  | 0x0080u
#define ID_API_MPLIST      ID_API_BASE  | 0x0100u
#define ID_API_IDM         ID_API_BASE  | 0x0200u
#define ID_API_MEM         ID_API_BASE  | 0x0400u
/* _pool.c I stands for internal */
#define ID_API_IPOOL       ID_API_BASE  | 0x0800u
#define ID_API_ALL         ID_API_PROC   | ID_API_CHNL   | ID_API_MSGQ   \
                        |  ID_API_POOL   | ID_API_NOTIFY | ID_API_MPCS   \
                        |  ID_API_RINGIO | ID_API_SYNC   | ID_API_MPLIST \
                        |  ID_API_IDM    | ID_API_MEM    | ID_API_IPOOL

/*  ============================================================================
 *  PMGR Subcomponent map
 *  ============================================================================
 */
#define ID_PMGR_PROC       ID_PMGR_BASE | 0x0001u
#define ID_PMGR_CHNL       ID_PMGR_BASE | 0x0002u
#define ID_PMGR_MSGQ       ID_PMGR_BASE | 0x0004u
#define ID_PMGR_ALL        ID_PMGR_PROC | ID_PMGR_CHNL | ID_PMGR_MSGQ

/*  ============================================================================
 *  GEN subcomponent map
 *  ============================================================================
 */
#define ID_GEN_UTILS        ID_GEN_BASE | 0x0001u
#define ID_GEN_LIST         ID_GEN_BASE | 0x0002u
#define ID_GEN_COFF         ID_GEN_BASE | 0x0004u
#define ID_GEN_COFF_55x     ID_GEN_BASE | 0x0008u
#define ID_GEN_COFF_64x     ID_GEN_BASE | 0x0010u
#define ID_GEN_STATICLOADER ID_GEN_BASE | 0x0020u
#define ID_GEN_BINLOADER    ID_GEN_BASE | 0x0040u
#define ID_GEN_IDM          ID_GEN_BASE | 0x0080u
#define ID_GEN_NOLOADER     ID_GEN_BASE | 0x0100u
#define ID_GEN_COFF_MEM     ID_GEN_BASE | 0x0200u
#define ID_GEN_COFF_FILE    ID_GEN_BASE | 0x0400u
#define ID_GEN_COFF_SHM     ID_GEN_BASE | 0x0800u
#define ID_GEN_ALL          ID_GEN_UTILS     | ID_GEN_LIST     | ID_GEN_COFF  \
                         |  ID_GEN_COFF_55x  | ID_GEN_COFF_64x                \
                         |  ID_GEN_STATICLOADER | ID_GEN_BINLOADER            \
                         |  ID_GEN_IDM | ID_GEN_NOLOADER | ID_GEN_COFF_MEM    \
                         |  ID_GEN_COFF_FILE | ID_GEN_COFF_SHM

/*  ============================================================================
 *  OSAL Subcomponent map
 *  ============================================================================
 */
#define ID_OSAL            ID_OSAL_BASE | 0x0001u
#define ID_OSAL_DPC        ID_OSAL_BASE | 0x0002u
#define ID_OSAL_ISR        ID_OSAL_BASE | 0x0004u
#define ID_OSAL_KFILE      ID_OSAL_BASE | 0x0008u
#define ID_OSAL_MEM        ID_OSAL_BASE | 0x0010u
#define ID_OSAL_PRCS       ID_OSAL_BASE | 0x0020u
#define ID_OSAL_SYNC       ID_OSAL_BASE | 0x0040u
#define ID_OSAL_TRC        ID_OSAL_BASE | 0x0080u
#define ID_OSAL_PRINT      ID_OSAL_BASE | 0x0100u
#define ID_OSAL_DRV        ID_OSAL_BASE | 0x0200u
#define ID_OSAL_NOTIFY_KNL ID_OSAL_BASE | 0x0400u
#define ID_OSAL_USER       ID_OSAL_BASE | 0x0800u
#define ID_OSAL_KFILE_PSEUDO ID_OSAL_BASE | 0x1000u
#define ID_OSAL_CLK        ID_OSAL_BASE | 0x2000u
#define ID_OSAL_REG        ID_OSAL_BASE | 0x4000u 
#define ID_OSAL_ALL        ID_OSAL      | ID_OSAL_DPC  | ID_OSAL_ISR      \
                         | ID_OSAL_KFILE| ID_OSAL_MEM  | ID_OSAL_PRCS     \
                         | ID_OSAL_SYNC | ID_OSAL_TRC  | ID_OSAL_PRINT    \
                         | ID_OSAL_DRV  | ID_OSAL_NOTIFY_KNL | ID_OSAL_USER \
                         | ID_OSAL_KFILE_PSEUDO | ID_OSAL_CLK | ID_OSAL_REG

/*  ============================================================================
 *  LDRV Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV            ID_LDRV_BASE | 0x0001u
#define ID_LDRV_PROC       ID_LDRV_BASE | 0x0002u
#define ID_LDRV_CHNL       ID_LDRV_BASE | 0x0004u
#define ID_LDRV_MSGQ       ID_LDRV_BASE | 0x0008u
#define ID_LDRV_CHIRPS     ID_LDRV_BASE | 0x0010u
#define ID_LDRV_MPCS       ID_LDRV_BASE | 0x0020u
#define ID_LDRV_MPLIST     ID_LDRV_BASE | 0x0040u
#define ID_LDRV_RINGIO     ID_LDRV_BASE | 0x0080u
#define ID_LDRV_SMM        ID_LDRV_BASE | 0x0100u
#define ID_LDRV_OS         ID_LDRV_BASE | 0x0200u
#define ID_LDRV_MPCS_OS    ID_LDRV_BASE | 0x0400u
#define ID_LDRV_ALL        ID_LDRV        | ID_LDRV_PROC   | ID_LDRV_CHNL     \
                         | ID_LDRV_MSGQ   | ID_LDRV_CHIRPS | ID_LDRV_MPCS     \
                         | ID_LDRV_MPLIST | ID_LDRV_RINGIO | ID_LDRV_SMM      \
                         | ID_LDRV_OS | ID_LDRV_MPCS_OS

/*  ============================================================================
 *  LDRV_DATA Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV_DATA       ID_LDRV_DATA_BASE | 0x0001u
#define ID_LDRV_DATA_PCPY  ID_LDRV_DATA_BASE | 0x0002u
#define ID_LDRV_DATA_ZCPY  ID_LDRV_DATA_BASE | 0x0004u
#define ID_LDRV_DATA_ALL   ID_LDRV_DATA | ID_LDRV_DATA_PCPY    \
                         | ID_LDRV_DATA_ZCPY

/*  ============================================================================
 *  LDRV_DRV Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV_DRV        ID_LDRV_DRV_BASE | 0x0001u
#define ID_LDRV_DRV_SHM    ID_LDRV_DRV_BASE | 0x0002u
#define ID_LDRV_DRV_ALL    ID_LDRV_DRV | ID_LDRV_DRV_SHM

/*  ============================================================================
 *  LDRV_IPS Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV_IPS        ID_LDRV_IPS_BASE | 0x0001u
#define ID_LDRV_IPS_IPS    ID_LDRV_IPS_BASE | 0x0002u
#define ID_LDRV_IPS_ALL    ID_LDRV_IPS | ID_LDRV_IPS_IPS

/*  ============================================================================
 *  LDRV_MQT Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV_MQT        ID_LDRV_MQT_BASE | 0x0001u
#define ID_LDRV_MQT_PCPY   ID_LDRV_MQT_BASE | 0x0002u
#define ID_LDRV_MQT_ZCPY   ID_LDRV_MQT_BASE | 0x0004u
#define ID_LDRV_MQT_ALL    ID_LDRV_MQT | ID_LDRV_MQT_PCPY | ID_LDRV_MQT_ZCPY

/*  ============================================================================
 *  LDRV_POOL Subcomponent map
 *  ============================================================================
 */
#define ID_LDRV_POOL       ID_LDRV_POOL_BASE | 0x0001u
#define ID_LDRV_POOL_BUF   ID_LDRV_POOL_BASE | 0x0002u
#define ID_LDRV_POOL_DMA   ID_LDRV_POOL_BASE | 0x0004u
#define ID_LDRV_POOL_SMA   ID_LDRV_POOL_BASE | 0x0008u
#define ID_LDRV_POOL_ALL   ID_LDRV_POOL      | ID_LDRV_POOL_BUF       \
                         | ID_LDRV_POOL_SMA  | ID_LDRV_POOL_DMA


/*  ============================================================================
 *  ARCH Subcomponent map
 *  ============================================================================
 */
#define ID_ARCH_DSP        ID_ARCH_BASE | 0x0001u
#define ID_ARCH_DSP_INTF   ID_ARCH_BASE | 0x0002u
#define ID_ARCH_CFG        ID_ARCH_BASE | 0x0004u
#define ID_ARCH_ALL        ID_ARCH_DSP | ID_ARCH_CFG

/*  ============================================================================
 *  ARCH_HAL subcomponent map
 *  ============================================================================
 */
#define ID_ARCH_HAL          ID_ARCH_HAL_BASE | 0x0001u
#define ID_ARCH_HAL_MAP      ID_ARCH_HAL_BASE | 0x0002u
#define ID_ARCH_HAL_INTGEN   ID_ARCH_HAL_BASE | 0x0004u
#define ID_ARCH_HAL_DMA      ID_ARCH_HAL_BASE | 0x0008u
#define ID_ARCH_HAL_RDWR     ID_ARCH_HAL_BASE | 0x0010u
#define ID_ARCH_HAL_BOOT     ID_ARCH_HAL_BASE | 0x0020u
#define ID_ARCH_HAL_PWR      ID_ARCH_HAL_BASE | 0x0040u
#define ID_ARCH_HAL_MMU      ID_ARCH_HAL_BASE | 0x0080u
#define ID_ARCH_DSPCLK       ID_ARCH_HAL_BASE | 0x0100u
#define ID_ARCH_HAL_ALL      ID_ARCH_HAL        | ID_ARCH_HAL_MAP              \
                           | ID_ARCH_HAL_INTGEN | ID_ARCH_HAL_DMA              \
                           | ID_ARCH_HAL_RDWR   | ID_ARCH_HAL_BOOT             \
                           | ID_ARCH_HAL_PWR      | ID_ARCH_HAL_MMU            \
                           | ID_ARCH_DSPCLK

/*  ============================================================================
 *  ARCH_PHY subcomponent map
 *  ============================================================================
 */
#define ID_ARCH_SHMEM_PHY    ID_ARCH_PHY_BASE | 0x0001u
#define ID_ARCH_PCI_PHY      ID_ARCH_PHY_BASE | 0x0002u
#define ID_ARCH_VLYNQ_PHY    ID_ARCH_PHY_BASE | 0x0004u
#define ID_ARCH_DSP_ALL      ID_ARCH_SHMEM_PHY | ID_ARCH_PCI_PHY              \
                           | ID_ARCH_VLYNQ_PHY

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (_SIGNATURE_H) */
