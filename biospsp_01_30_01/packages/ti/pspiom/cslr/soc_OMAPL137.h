/*
 * soc_OMAPL137.h
 *
 * This file contains the peripheral registers base addresseses, peripheral 
 * counts, peripheral inettupr details etc for the respective SoC
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

#ifndef _SOC_OMAPL137_H_
#define _SOC_OMAPL137_H_

/* =============================================================================
 *  Revision History
 *  ===============
 *  15-Feb-2007 NS  File created.
 *  Aug 27, 2008           SRIRAM M       Updated for OMAPL137 SoC
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <ti/pspiom/cslr/cslr.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL137.h>

/**************************************************************************\
* SOC file
\**************************************************************************/

/******************************************************************************\
* Static inline definition
\******************************************************************************/
#ifndef CSL_IDEF_INLINE
#define CSL_IDEF_INLINE static inline
#endif

/******************************************************************************\
* Peripheral Instance count
\******************************************************************************/

/** @brief Number of UHPI instances                                           */
#define CSL_HPI_PER_CNT                     1

/** @brief Number of McASP instances                                          */
#define CSL_MCASP_PER_CNT                   3

/** @brief Number of TIMER instances                                          */
#define CSL_TMR_PER_CNT                     2

/** @brief Number of PSC instances                                            */
#define CSL_PSC_PER_CNT                     2

/** @brief Number of UART instances                                           */
#define CSL_UART_PER_CNT                    3

/** @brief Number of SPI instances                                            */
#define CSL_SPI_PER_CNT                     2

/** @brief Number of I2C instances                                            */
#define CSL_I2C_PER_CNT                     2

/** @brief Number of PLL instances                                            */
#define CSL_PLLC_PER_CNT                    1

/** @brief Number of MMCSD instances                                          */
#define CSL_MMCSD_PER_CNT                   1

/** @brief Number of LCDC instances                                            */
#define CSL_LCDC_PER_CNT                     1

/** @brief Number of EDMA3 CC instances                                       */
#define CSL_EDMA3CC_CNT                     1

/** @brief Number of EDMA3 TC instances                                       */
#define CSL_EDMA3TC_CNT                     2

/** @brief Number of EMIFA instances                                          */
#define CSL_EMIFA_PER_CNT                   1

/** @brief Number of EMIFB instances                                          */
#define CSL_EMIFB_PER_CNT                   1

/** @brief Number of EMAC instances                                           */
#define CSL_EMAC_PER_CNT                    1

/** @brief Number of MDIO instances                                           */
#define CSL_MDIO_PER_CNT                    1

/** @brief Number of EHRPWM instances                                         */
#define CSL_EHRPWM_PER_CNT                  3

/** @brief Number of ECAP instances                                           */
#define CSL_ECAP_PER_CNT                    3

/** @brief Number of EQEP instances                                           */
#define CSL_EQEP_PER_CNT                    2

/** @brief Number of USB instances                              */
#define CSL_USB_PER_CNT                     2

/** @brief Number of INTC instances                             */
#define CSL_INTC_PER_CNT                    1

/** @brief Number of AINTC instances                            */
#define CSL_AINTC_PER_CNT                   1

/** @brief Number of RTC instances                              */
#define CSL_RTC_PER_CNT                     1

/** @brief Number of GPIO instances                             */
#define CSL_GPIO_PER_CNT                    1

/** @brief Number of ECTL instances                             */
#define CSL_ECTL_PER_CNT                    1

/** @brief Number of SYSCFG instances                           */
#define CSL_SYSCFG_PER_CNT                  1
/******************************************************************************\
* Peripheral Instance definitions.
\******************************************************************************/

/** @brief Peripheral Instances of UHPI instances                             */
#define CSL_HPI                             (0)

/** @brief Peripheral Instances of McASP instances                            */
#define CSL_MCASP_0                         (0)
#define CSL_MCASP_1                         (1)
#define CSL_MCASP_2                         (2)

/** @brief Peripheral Instance of EDMA CC instances                           */
#define CSL_EDMA3CC                         (0)

/** @brief Peripheral Instance of EDMA TC instances                           */
#define CSL_EDMA3TC_0                       (0)
#define CSL_EDMA3TC_1                       (1)

/** @brief Peripheral Instance of Timer 64 instances                          */
#define CSL_TMR_0                           (0)
#define CSL_TMR_1                           (1)

/** @brief Peripheral Instances of PSC instances                              */
#define CSL_PSC_0                           (0)
#define CSL_PSC_1                           (1)

/** @brief Peripheral Instances of UART instances                             */
#define CSL_UART_0                          (0)
#define CSL_UART_1                          (1)
#define CSL_UART_2                          (2)

/** @brief Peripheral Instances of SPI instances                              */
#define CSL_SPI_0                           (0)
#define CSL_SPI_1                           (1)

/** @brief Peripheral Instances of I2C instances                              */
#define CSL_I2C_0                           (0)
#define CSL_I2C_1                           (1)

/** @brief Peripheral Instances of MMCSD instances                            */
#define CSL_MMCSD_0                         (0)

/** @brief Peripheral Instances of LCDC instances                              */
#define CSL_LCDC                            (0)

/** @brief Instance number of PLL controller                                  */
#define CSL_PLLC                            (0)

/** @brief Peripheral Instance of EMIFA instances                             */
#define CSL_EMIFA                           (0)

/** @brief Peripheral Instance of EMIFB instances                             */
#define CSL_EMIFB                           (0)

/** @brief Peripheral Instance of EMAC instances                              */
#define CSL_EMAC                            (0)

/** @brief Peripheral Instance of MDIO instances                              */
#define CSL_MDIO                            (0)

/** @brief Peripheral Instance of EHRPWM instances                            */
#define CSL_EHRPWM_0                        (0)
#define CSL_EHRPWM_1                        (1)
#define CSL_EHRPWM_2                        (2)

/** @brief Peripheral Instance of ECAP instances                              */
#define CSL_ECAP_0                          (0)
#define CSL_ECAP_1                          (1)
#define CSL_ECAP_2                          (2)

/** @brief Peripheral Instance of EQEP instances                              */
#define CSL_EQEP_0                          (0)
#define CSL_EQEP_1                          (1)

/** @brief Peripheral Instance of USB instances                               */
#define CSL_USB_0                           (0)
#define CSL_USB_1                           (1)

/** @brief Peripheral Instance of INTC instances                              */
#define CSL_INTC                            (0)

/** @brief Peripheral Instance of AINTC instances                             */
#define CSL_AINTC                           (0)

/** @brief Peripheral Instance of RTC instances                               */
#define CSL_RTC                             (0)

/** @brief Peripheral Instance of GPIO instances                              */
#define CSL_GPIO                            (0)
/** @brief GPIO pin and bank information                                      */
#define CSL_GPIO_NUM_PINS                   (128)
#define CSL_GPIO_NUM_BANKS                  ((CSL_GPIO_NUM_PINS + 15)/16)

/** @brief Peripheral Instance of ECTL instances                              */
#define CSL_ECTL                            (0)

/** @brief Peripheral Instance of SYSCFG instances                            */
#define CSL_SYSCFG                          (0)

/*******************************************************************************
* Peripheral Base Address
*******************************************************************************/

/** @brief Base address of INTC memory mapped registers                       */
#define CSL_INTC_0_REGS                     (0x01800000u)

/** @brief Base address of AINTC memory mapped registers                      */
#define CSL_AINTC_0_REGS                    (0xFFFEE000u)

/** #brief Cache Module memory mapped address                                 */
#define CSL_CACHE_0_REGS                    (0x01840000u)

/** @brief Base address of PDC memory mapped registers                        */
#define CSL_PWRDWN_PDC_REGS                 (0x01810000u)

/** @brief Base address of SYS - Security ID register                         */
#define CSL_SYS_0_SECURITY_ID_REGS          (0x01811000u)

/** @brief Base address of SYS - Revision ID register                         */
#define CSL_SYS_0_REV_ID_REGS               (0x01812000u)

/** #brief IDMA Module memory mapped address  */
#define CSL_IDMA_0_REGS                     (0x01820000u)

/** #brief EMC Module memory mapped address                                   */
#define CSL_EMC_0_REGS                      (0x01820000u)

/** @brief Base address of UMC Memory protection registers                    */
#define CSL_MEMPROT_L2_REGS                 (0x00800000u)

/** @brief Base address of PMC memory Protection registers                    */
#define CSL_MEMPROT_L1P_REGS                (0x00E00000u)

 /** @brief Base address of DMC memory protection registers                   */
#define CSL_MEMPROT_L1D_REGS                (0x00F00000u)

/** @brief Base address of HPI memory mapped registers                        */
#define CSL_HPI_0_REGS                      (0x01E10000u)

/** @brief Base address of McASP memory mapped registers                      */
#define CSL_MCASP_0_CTRL_REGS               (0x01D00000u)
#define CSL_MCASP_0_FIFO_REGS               (0x01D01000u)
#define CSL_MCASP_0_DATA_REGS               (0x01D02000u)
#define CSL_MCASP_1_CTRL_REGS               (0x01D04000u)
#define CSL_MCASP_1_FIFO_REGS               (0x01D05000u)
#define CSL_MCASP_1_DATA_REGS               (0x01D06000u)
#define CSL_MCASP_2_CTRL_REGS               (0x01D08000u)
#define CSL_MCASP_2_FIFO_REGS               (0x01D09000u)
#define CSL_MCASP_2_DATA_REGS               (0x01D0A000u)

/** @brief Base address of TIMER memory mapped registers                      */
#define CSL_TMR_0_REGS                      (0x01C20000u)
#define CSL_TMR_1_REGS                      (0x01C21000u)

/** @brief Base address of PSC memory mapped registers                        */
#define CSL_PSC_0_REGS                      (0x01C10000u)
#define CSL_PSC_1_REGS                      (0x01E27000u)

/** @brief Base address of UART memory mapped registers                       */
#define CSL_UART_0_REGS                     (0x01C42000u)
#define CSL_UART_1_REGS                     (0x01D0C000u)
#define CSL_UART_2_REGS                     (0x01D0D000u)

/** @brief Base address of SPI memory mapped registers                        */
#define CSL_SPI_0_REGS                      (0x01C41000u)
#define CSL_SPI_1_REGS                      (0x01E12000u)

/** @brief Base address of I2C memory mapped registers                        */
#define CSL_I2C_0_REGS                      (0x01C22000u)
#define CSL_I2C_1_REGS                      (0x01E28000u)

/** @brief Base address of GPIO memory mapped registers                       */
#define CSL_GPIO_0_REGS                     (0x01E26000u)

/** @brief Base address of DEV memory mapped registers                        */
#define CSL_SYSCFG_0_REGS                   (0x01C14000u)

/** @brief Base address of MMCSD memory mapped registers                      */
#define CSL_MMCSD_0_REGS                    (0x01C40000)

/** @brief Base address of LCDC memory mapped registers                        */
#define CSL_LCDC_0_REGS                     (0x01E13000u)

/** @brief PLL controller instance 1 module address                           */
#define CSL_PLLC_0_REGS                     (0x01C11000u)

/** @brief Base address of Channel controller  memory mapped registers        */
#define CSL_EDMA3CC_0_REGS                  (0x01C00000u)

/** @brief Base address of Transfer controller  memory mapped registers       */
#define CSL_EDMA3TC_0_REGS                  (0x01C08000u)
#define CSL_EDMA3TC_1_REGS                  (0x01C08400u)

/** @brief Base address of EPWM memory mapped registers                       */
#define CSL_EHRPWM_0_REGS                   (0x01F00000u)
#define CSL_EHRPWM_1_REGS                   (0x01F02000u)
#define CSL_EHRPWM_2_REGS                   (0x01F04000u)

/** @brief Base address of MDIO memory mapped registers                       */
#define CSL_MDIO_0_REGS                     (0x01E24000u)

/** @brief Base address of ECAP memory mapped registers                       */
#define CSL_ECAP_0_REGS                     (0x01F06000u)
#define CSL_ECAP_1_REGS                     (0x01F07000u)
#define CSL_ECAP_2_REGS                     (0x01F08000u)

/** @brief Base address of EQEP memory mapped registers                       */
#define CSL_EQEP_0_REGS                     (0x01F09000u)
#define CSL_EQEP_1_REGS                     (0x01F0A000u)

/** @brief Base address of EMIFA memory mapped registers                      */
#define CSL_EMIFA_0_REGS                    (0x68000000u)

/** @brief Base address of EMIFA_CS0 memory                                   */
#define CSL_EMIFA_CS0_ADDR                  (0x40000000u)

/** @brief Base address of EMIFA_CS2 memory                                   */
#define CSL_EMIFA_CS2_ADDR                  (0x60000000u)

/** @brief Base address of EMIFA_CS3 memory                                   */
#define CSL_EMIFA_CS3_ADDR                  (0x62000000u)

/** @brief Base address of EMIFA_CS4 memory                                   */
#define CSL_EMIFA_CS4_ADDR                  (0x64000000u)

/** @brief Base address of EMIFA_CS5 memory                                   */
#define CSL_EMIFA_CS5_ADDR                  (0x66000000u)

/** @brief Base address of EMIFB Control Registers                            */
#define CSL_EMIFB_0_REGS                    (0xB0000000u)

/** @brief Base address of EMIFB SDRAM memory                                 */
#define CSL_EMIFB_SDRAM_ADDR                (0xC0000000u)

/** @brief Base address of USB memory                                         */
#define CSL_USB_0_REGS                      (0x01E00000u)
#define CSL_USB_1_REGS                      (0x01E25000u)

#define CSL_MPU_0_REGS                      (0x01E14000u)
#define CSL_MPU_1_REGS                      (0x01E15000u)

/** @brief Base address of EMAC memory                                        */
#define CSL_EMAC_DSC_CTRL_MOD_RAM           (0x01E20000u)
#define CSL_EMAC_DSC_CONTROL_REG            (0x01E23000u)

/** @brief Base address of ECTL memory                                        */
#define CSL_ECTL_0_REGS                   (0x01E22000u)

/** @brief Base address of RTC memory                                         */
#define CSL_RTC_0_REGS                    (0x01C23000u)

/******************************************************************************\
* Interrupt Event IDs
\******************************************************************************/

/**
 * @brief   Interrupt Event IDs
 */
/* Output of event combiner 0, for events 1 to 31                             */
#define    CSL_INTC_EVENTID_EVT0            (0)
/* Output of event combiner 0, for events 32 to 63                            */
#define    CSL_INTC_EVENTID_EVT1            (1)
/* Output of event combiner 0, for events 64 to 95                            */
#define    CSL_INTC_EVENTID_EVT2            (2)
/* Output of event combiner 0, for events 96 to 127                           */
#define    CSL_INTC_EVENTID_EVT3            (3)

/* BOOTCFG CHIPSIG Register Interrupt                                         */
#define    CSL_INTC_EVENTID_BOOTCFGINT0     (5)
#define    CSL_INTC_EVENTID_CHIPINT3        (67)

/* HiResTimer Interrupt                                                       */
#define    CSL_INTC_EVENTID_EHRPWM0         (7)
#define    CSL_INTC_EVENTID_EHRPWM1         (18)
#define    CSL_INTC_EVENTID_EHRPWM2         (24)

/* TPCC0 Region 1 Interrupt                                                   */
#define    CSL_INTC_EVENTID_TPCC0_INT1      (8)

/* EMU interrupt for                                                          */
#define    CSL_INTC_EVENTID_EMU_DTDMA       (9)

/* HiResTimer Trip Zone Interrupt                                             */
#define    CSL_INTC_EVENTID_EHRPWM0TZ         (10)
#define    CSL_INTC_EVENTID_EHRPWM1TZ         (23)
#define    CSL_INTC_EVENTID_EHRPWM2TZ         (25)

/* EMU real time data exchange receive complete                               */
#define    CSL_INTC_EVENTID_EMU_RTDXRX      (11)
/* EMU RTDX transmit complete                                                 */
#define    CSL_INTC_EVENTID_EMU_RTDXTX      (12)
/* IDMA Channel 0 Interrupt                                                   */
#define    CSL_INTC_EVENTID_IDMA0           (13)
/* IDMA Channel 1 Interrupt                                                   */
#define    CSL_INTC_EVENTID_IDMA1           (14)

/* MMCSD/SDIO interrupt                                                       */
#define    CSL_INTC_EVENTID_MMCSDINT0       (15)
#define    CSL_INTC_EVENTID_MMCSDINT1       (16)

/* USB Interrupt                                                              */
#define    CSL_INTC_EVENTID_USBINT0         (19)
#define    CSL_INTC_EVENTID_USB1_HCINT      (20)
#define    CSL_INTC_EVENTID_USB1_RWAKEUP    (21)

/* EMAC Interrupt                                                             */
#define     CSL_INTC_EVENTID_EMAC_RXTHRHC0  (26)
#define     CSL_INTC_EVENTID_EMAC_RXTHRHC1  (30)
#define     CSL_INTC_EVENTID_EMAC_RXC0      (27)
#define     CSL_INTC_EVENTID_EMAC_RXC1      (31)
#define     CSL_INTC_EVENTID_EMAC_TXC0      (28)
#define     CSL_INTC_EVENTID_EMAC_TXC1      (32)
#define     CSL_INTC_EVENTID_EMAC_MISCC0    (29)
#define     CSL_INTC_EVENTID_EMAC_MISCC1    (33)

/* UHPI DSPI Interrupt                                                        */
#define     CSL_INTC_EVENTID_UHPI_DSPINT    (34)

/* I2C interrupt                                                              */
#define    CSL_INTC_EVENTID_I2CINT0         (36)
#define    CSL_INTC_EVENTID_I2CINT1         (42)


/* EMIFA Error Interrupt                                                      */
#define    CSL_INTC_EVENTID_EMIFAINT        (55)
#define    CSL_INTC_EVENTID_EMIFBINT        (60)

/* Timer 64P0 lower counter interrupt                                         */
#define    CSL_INTC_EVENTID_T64P0_TINT12    (4)
/* Timer 64P0 higher counter interrupt                                        */
#define    CSL_INTC_EVENTID_T64P0_TINT34    (64)
/* Timer 64P1 lower counter interrupt                                         */
#define    CSL_INTC_EVENTID_T64P1_TINT12    (40)
/* Timer 64P1 higher counter interrupt                                        */
#define    CSL_INTC_EVENTID_T64P1_TINT34    (48)

/* PSC interrupt                                                              */
#define    CSL_INTC_EVENTID_PSC0_ALLINT     (70)
#define    CSL_INTC_EVENTID_PSC1_ALLINT     (71)
/* LCDC interrupt                                                             */
#define    CSL_INTC_EVENTID_LCDC_INT0       (73)
/* SPI interrupt                                                              */
#define    CSL_INTC_EVENTID_SPIINT0         (37)
/* SPI interrupt                                                              */
#define    CSL_INTC_EVENTID_SPIINT1         (43)
/* UART interrupt                                                             */
#define    CSL_INTC_EVENTID_UARTINT0        (38)
#define    CSL_INTC_EVENTID_UARTINT1        (46)
#define    CSL_INTC_EVENTID_UARTINT2        (69)

/* HiResTimer/PWM Interrupt                                                   */
#define    CSL_INTC_EVENTID_ECAPINT0        (45)
#define    CSL_INTC_EVENTID_ECAPINT1        (47)
#define    CSL_INTC_EVENTID_ECAPINT2        (51)
#define    CSL_INTC_EVENTID_EQEPINT0        (68)
#define    CSL_INTC_EVENTID_EQEPINT1        (53)

/* EDMA3 CC Interrupt                                                         */
#define    CSL_INTC_EVENTID_TPCC0_ERRINT    (56)
/* EDMA3 TC Interrupt                                                         */
#define    CSL_INTC_EVENTID_TPTC_ERRINT0    (57)
#define    CSL_INTC_EVENTID_TPTC_ERRINT1    (58)

/* GPIO BANK 1 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK0_INT   (65)
#define    CSL_INTC_EVENTID_GPIO_BNK1_INT   (41)
#define    CSL_INTC_EVENTID_GPIO_BNK2_INT   (49)
#define    CSL_INTC_EVENTID_GPIO_BNK3_INT   (52)
#define    CSL_INTC_EVENTID_GPIO_BNK4_INT   (54)
#define    CSL_INTC_EVENTID_GPIO_BNK5_INT   (59)
#define    CSL_INTC_EVENTID_GPIO_BNK6_INT   (62)
#define    CSL_INTC_EVENTID_GPIO_BNK7_INT   (72)

/* McASP TX interrupt                                                         */
#define    CSL_INTC_EVENTID_AXRINT          (61)
/* RTC Combined Interrupt                                                     */
#define    CSL_INTC_EVENTID_RTC_IRQS        (63)
/* Protection Shared Interrupt                                                */
#define    CSL_INTC_EVENTID_PROTERR         (74)
/* SECCTL or KEYMGR Error Interrupt                                           */
#define    CSL_INTC_EVENTID_SECKEYERR       (76)

/* Timer 64P0 Compare Interrupt                                               */
#define    CSL_INTC_EVENTID_T64P0_CMPINT0   (78)
#define    CSL_INTC_EVENTID_T64P0_CMPINT1   (79)
#define    CSL_INTC_EVENTID_T64P0_CMPINT2   (80)
#define    CSL_INTC_EVENTID_T64P0_CMPINT3   (81)
#define    CSL_INTC_EVENTID_T64P0_CMPINT4   (82)
#define    CSL_INTC_EVENTID_T64P0_CMPINT5   (83)
#define    CSL_INTC_EVENTID_T64P0_CMPINT6   (84)
#define    CSL_INTC_EVENTID_T64P0_CMPINT7   (85)

/* Timer 64P1 Compare Interrupt                                               */
#define    CSL_INTC_EVENTID_T64P1_CMPINT0   (86)
#define    CSL_INTC_EVENTID_T64P1_CMPINT1   (87)
#define    CSL_INTC_EVENTID_T64P1_CMPINT2   (88)
#define    CSL_INTC_EVENTID_T64P1_CMPINT3   (89)
#define    CSL_INTC_EVENTID_T64P1_CMPINT4   (90)
#define    CSL_INTC_EVENTID_T64P1_CMPINT5   (91)
#define    CSL_INTC_EVENTID_T64P1_CMPINT6   (92)
#define    CSL_INTC_EVENTID_T64P1_CMPINT7   (93)


/* CC Completion Interrupt for region 1 */
#define    CSL_INTC_EVENTID_EDMA3CC_INT1    (8)
/* CC Error Interrupt    */
#define    CSL_INTC_EVENTID_EDMA3CC_ERRINT  (120)
/* TC0 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC0_ERRINT (121)
/* TC1 Error Interrupt */
#define    CSL_INTC_EVENTID_EDMA3TC1_ERRINT (122)

/* Dropped CPU interrupt event                                                */
#define    CSL_INTC_EVENTID_INTERR          (96)
/* EMC Invalid IDMA parameters                                                */
#define    CSL_INTC_EVENTID_EMC_IDMAERR     (97)

/* PMC Single bit error detected during DMA read                              */
#define    CSL_INTC_EVENTID_PMC_ED          (113)

/* UMC single bit error detected                                              */
#define    CSL_INTC_EVENTID_UMC_ED1         (116)
/* UMC two bit error detected                                                 */
#define    CSL_INTC_EVENTID_UMC_ED2         (117)
/* Power Down sleep interrupt                                                 */
#define    CSL_INTC_EVENTID_PDC_INT         (118)

/* SYS CMPA CPU memory protection fault                                       */
#define    CSL_INTC_EVENTID_SYS_CMPA        (119)

/* PMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_PMC_CMPA        (120)
/* PMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_PMC_DMPA        (121)
/* DMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_DMC_CMPA        (122)
/* DMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_DMC_DMPA        (123)
/* UMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_UMC_CMPA        (124)
/* UMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_UMC_DMPA        (125)
/* IDMA CPU memory protection fault                                           */
#define    CSL_INTC_EVENTID_EMC_CMPA        (126)
/* IDMA Bus error interrupt                                                   */
#define    CSL_INTC_EVENTID_EMC_BUSERR      (127)



/*************************** LPSC Numbers of peripherals **********************/
/* Some of the SoC parts support Power management and hence need to use PWRM
 * specific Macro. To have the common code base between the SOC parts,        *
 * even though this are defined in cslr_psc<soc>.h                            *
 * as in some SOC part PWRM specific macros hasto be                          */
 
/* Peripherals in PSC0                                                        */

#define    CSL_LPSC_NUMBER_EDMA3_0_CC_0         CSL_PSC_CC
#define    CSL_LPSC_NUMBER_EDMA3_0_TC_0         CSL_PSC_TC0
#define    CSL_LPSC_NUMBER_EDMA3_0_TC_1         CSL_PSC_TC1
#define    CSL_LPSC_NUMBER_EMIFA_0              CSL_PSC_EMIFA
#define    CSL_LPSC_NUMBER_SPI_0                CSL_PSC_SPI0
#define    CSL_LPSC_NUMBER_MMCSD_0              CSL_PSC_MMCSD0
#define    CSL_LPSC_NUMBER_AINTC_0              CSL_PSC_AINTC
#define    CSL_LPSC_NUMBER_ARM_RAMROM_0         CSL_PSC_ARM_RAMROM
#define    CSL_LPSC_NUMBER_SECCNTL_KEYMGR_0     CSL_PSC_SECCNTL_KEYMGR
#define    CSL_LPSC_NUMBER_UART_0               CSL_PSC_UART0
#define    CSL_LPSC_NUMBER_SCR_0                CSL_PSC_SCR0_SS
#define    CSL_LPSC_NUMBER_SCR_1                CSL_PSC_SCR1_SS
#define    CSL_LPSC_NUMBER_SCR_2                CSL_PSC_SCR2_SS
#define    CSL_LPSC_NUMBER_ARM                  CSL_PSC_ARM
#define    CSL_LPSC_NUMBER_DSP                  CSL_PSC_DSP


/* Peripherals in PSC1                                                        */    
#define    CSL_LPSC_NUMBER_USB_0                CSL_PSC_USB0
#define    CSL_LPSC_NUMBER_USB_1                CSL_PSC_USB1
#define    CSL_LPSC_NUMBER_GPIO_0               CSL_PSC_GPIO
#define    CSL_LPSC_NUMBER_UHPI_0               CSL_PSC_UHPI
#define    CSL_LPSC_NUMBER_EMAC_0               CSL_PSC_EMAC
#define    CSL_LPSC_NUMBER_EMIFB_0              CSL_PSC_EMIFB
#define    CSL_LPSC_NUMBER_MCASP_0              CSL_PSC_MCASP0
#define    CSL_LPSC_NUMBER_MCASP_1              CSL_PSC_MCASP1
#define    CSL_LPSC_NUMBER_MCASP_2              CSL_PSC_MCASP2
#define    CSL_LPSC_NUMBER_SPI_1                CSL_PSC_SPI1
#define    CSL_LPSC_NUMBER_I2C_1                CSL_PSC_I2C1
#define    CSL_LPSC_NUMBER_UART_1               CSL_PSC_UART1
#define    CSL_LPSC_NUMBER_UART_2               CSL_PSC_UART2
#define    CSL_LPSC_NUMBER_LCDC_0               CSL_PSC_LCDC
#define    CSL_LPSC_NUMBER_EHRPWM_0             CSL_PSC_EHRPWM0_1_2
#define    CSL_LPSC_NUMBER_EHRPWM_1             CSL_PSC_EHRPWM0_1_2
#define    CSL_LPSC_NUMBER_EHRPWM_2             CSL_PSC_EHRPWM0_1_2
#define    CSL_LPSC_NUMBER_ECAP_0               CSL_PSC_ECAP0_1_2
#define    CSL_LPSC_NUMBER_ECAP_1               CSL_PSC_ECAP0_1_2
#define    CSL_LPSC_NUMBER_EQEP_0               CSL_PSC_EQEP0_1_2
#define    CSL_LPSC_NUMBER_EQEP_1               CSL_PSC_EQEP0_1_2
#define    CSL_LPSC_NUMBER_EDMA3_1_TC_1         CSL_PSC_TC2
#define    CSL_LPSC_NUMBER_SCRF_7               CSL_PSC_SCRF7_SS
#define    CSL_LPSC_NUMBER_SCRF_8               CSL_PSC_SCRF8_SS
#define    CSL_LPSC_NUMBER_SHRAM                CSL_PSC_SHRAM


#define    CSL_LPSC_INSTANCE_MAX            2u
#define    CSL_LPSC_NUMBER_MAX              32u

/************************  PSC instances of peripherals **********************/
/* In some SOCs, the peripherals are spread across more that one LPSC. Hence *
 * the knowledge of the PSC instance on which they reside is important for   *
 * driver                                                                    */ 

#define    CSL_LPSC_INST_EDMA3_0_CC_0           CSL_PSC_0
#define    CSL_LPSC_INST_EDMA3_0_TC_0           CSL_PSC_0
#define    CSL_LPSC_INST_EDMA3_0_TC_1           CSL_PSC_0
#define    CSL_LPSC_INST_EMIFA_0                CSL_PSC_0
#define    CSL_LPSC_INST_SPI_0                  CSL_PSC_0
#define    CSL_LPSC_INST_MMCSD_0                CSL_PSC_0
#define    CSL_LPSC_INST_AINTC_0                CSL_PSC_0
#define    CSL_LPSC_INST_ARM_RAMROM_0           CSL_PSC_0
#define    CSL_LPSC_INST_SECCNTL_KEYMGR_0       CSL_PSC_0
#define    CSL_LPSC_INST_UART_0                 CSL_PSC_0
#define    CSL_LPSC_INST_SCR_0                  CSL_PSC_0
#define    CSL_LPSC_INST_SCR_1                  CSL_PSC_0
#define    CSL_LPSC_INST_SCR_2                  CSL_PSC_0
#define    CSL_LPSC_INST_ARM                    CSL_PSC_0
#define    CSL_LPSC_INST_DSP                    CSL_PSC_0



/* Peripherals in PSC0                                                        */
#define    CSL_LPSC_INST_USB_0                  CSL_PSC_1
#define    CSL_LPSC_INST_USB_1                  CSL_PSC_1
#define    CSL_LPSC_INST_GPIO_0                 CSL_PSC_1
#define    CSL_LPSC_INST_UHPI_0                 CSL_PSC_1
#define    CSL_LPSC_INST_EMAC_0                 CSL_PSC_1
#define    CSL_LPSC_INST_EMIFB_0                CSL_PSC_1
#define    CSL_LPSC_INST_MCASP_0                CSL_PSC_1
#define    CSL_LPSC_INST_MCASP_1                CSL_PSC_1
#define    CSL_LPSC_INST_MCASP_2                CSL_PSC_1
#define    CSL_LPSC_INST_SPI_1                  CSL_PSC_1
#define    CSL_LPSC_INST_I2C_1                  CSL_PSC_1
#define    CSL_LPSC_INST_UART_1                 CSL_PSC_1
#define    CSL_LPSC_INST_UART_2                 CSL_PSC_1
#define    CSL_LPSC_INST_LCDC_0                 CSL_PSC_1
#define    CSL_LPSC_INST_EHRPWM_0               CSL_PSC_1
#define    CSL_LPSC_INST_EHRPWM_1               CSL_PSC_1
#define    CSL_LPSC_INST_EHRPWM_2               CSL_PSC_1
#define    CSL_LPSC_INST_ECAP_0                 CSL_PSC_1
#define    CSL_LPSC_INST_ECAP_1                 CSL_PSC_1
#define    CSL_LPSC_INST_EQEP_0                 CSL_PSC_1
#define    CSL_LPSC_INST_EQEP_1                 CSL_PSC_1
#define    CSL_LPSC_INST_EDMA3_1_TC_1           CSL_PSC_1
#define    CSL_LPSC_INST_SCRF_7                 CSL_PSC_1
#define    CSL_LPSC_INST_SCRF_8                 CSL_PSC_1
#define    CSL_LPSC_INST_SHRAM                  CSL_PSC_1

/*************************** EDMA RELATED DEFINES  ****************************/


/******************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL
*  parameters for the given SOC
\******************************************************************************/

#define CSL_EDMA3_NUM_DMACH                 64
#define CSL_EDMA3_NUM_QDMACH                8
#define CSL_EDMA3_NUM_PARAMSETS             128
#define CSL_EDMA3_NUM_EVQUE                 2
#define CSL_EDMA3_CHMAPEXIST                0
#define CSL_EDMA3_NUM_REGIONS               4
#define CSL_EDMA3_MEMPROTECT                0

/******************************************************************************\
* Channel Instance count
\******************************************************************************/
#define CSL_EDMA3_CHA_CNT                   (CSL_EDMA3_NUM_DMACH + \
                                             CSL_EDMA3_NUM_QDMACH)

/* EDMA channel synchronization events                                        */

/* McASP0 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCASP0_RX             0
/* McASP0 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCASP0_TX             1
/* McASP1 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCASP1_RX             2
/* McASP1 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCASP1_TX             3
/* McASP2 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCASP2_RX             4
/* McASP2 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCASP2_TX             5

/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT0          6
/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT1          7
/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT2          22
/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT3          23
/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT4          28
/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT5          29

/* UART0 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART0_RX              8
/* UART0 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART0_TX              9
/* UART1 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART1_RX              12
/* UART1 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART1_TX              13
/* UART2 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART2_RX              30
/* UART2 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART2_TX              31

/* Timer 64P Event Out 12                                                     */
#define CSL_EDMA3_CHA_TIMER64P_EVT12        10
/* Timer 64P Event Out 34                                                     */
#define CSL_EDMA3_CHA_TIMER64P_EVT34        11

/* SPI0 Receive Event                                                         */
#define CSL_EDMA3_CHA_SPI0_RX               14
/* SPI0 Transmit Event                                                        */
#define CSL_EDMA3_CHA_SPI0_TX               15
/* SPI1 Receive Event                                                         */
#define CSL_EDMA3_CHA_SPI1_RX               18
/* SPI1 Transmit Event                                                        */
#define CSL_EDMA3_CHA_SPI1_TX               19

/* MMCSD Receive Event                                                        */
#define CSL_EDMA3_CHA_MMCSD_RX              16
/* MMCSD Transmit Event                                                       */
#define CSL_EDMA3_CHA_MMCSD_TX              17

/* I2C0 Receive Event                                                         */
#define CSL_EDMA3_CHA_I2C0_RX               24
/* I2C0 Transmit Event                                                        */
#define CSL_EDMA3_CHA_I2C0_TX               25
/* I2C1 Receive Event                                                         */
#define CSL_EDMA3_CHA_I2C1_RX               26
/* I2C1 Transmit Event                                                        */
#define CSL_EDMA3_CHA_I2C1_TX               27



/* QDMA channels                                                              */
#define    CSL_EDMA3_QCHA_BASE              CSL_EDMA3_NUM_DMACH         /* QDMA Channel Base */
#define    CSL_EDMA3_QCHA_0                 (CSL_EDMA3_QCHA_BASE + 0)   /* QDMA Channel 0 */
#define    CSL_EDMA3_QCHA_1                 (CSL_EDMA3_QCHA_BASE + 1)   /* QDMA Channel 1 */
#define    CSL_EDMA3_QCHA_2                 (CSL_EDMA3_QCHA_BASE + 2)   /* QDMA Channel 2 */
#define    CSL_EDMA3_QCHA_3                 (CSL_EDMA3_QCHA_BASE + 3)   /* QDMA Channel 3 */
#define    CSL_EDMA3_QCHA_4                 (CSL_EDMA3_QCHA_BASE + 4)   /* QDMA Channel 4 */
#define    CSL_EDMA3_QCHA_5                 (CSL_EDMA3_QCHA_BASE + 5)   /* QDMA Channel 5 */
#define    CSL_EDMA3_QCHA_6                 (CSL_EDMA3_QCHA_BASE + 6)   /* QDMA Channel 6 */
#define    CSL_EDMA3_QCHA_7                 (CSL_EDMA3_QCHA_BASE + 7)   /* QDMA Channel 7 */

/* Enumerations for EDMA Event Queues */
typedef enum {
    CSL_EDMA3_QUE_0            =            0, /* Queue 0                     */
    CSL_EDMA3_QUE_1            =            1  /* Queue 1                     */
} CSL_Edma3Que;

/* Enumerations for EDMA Transfer Controllers
 *
 * There are 2 Transfer Controllers. Typically a one to one mapping exists
 * between Event Queues and Transfer Controllers.
 *
 */
typedef enum {
    CSL_EDMA3_TC_0         =                0, /* TC 0                        */
    CSL_EDMA3_TC_1         =                1  /* TC 1                        */
} CSL_Edma3Tc;

#define CSL_EDMA3_REGION_GLOBAL            -1
#define CSL_EDMA3_REGION_0                  0
#define CSL_EDMA3_REGION_1                  1
#define CSL_EDMA3_REGION_2                  2
#define CSL_EDMA3_REGION_3                  3



/******************************* DAT RELATED DEFINES **************************/

/******************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL
*  parameters for the given SOC
\******************************************************************************/
/******************************************************************************\
* Channel Instance count
\******************************************************************************/
/** @brief Number of Generic Channel instances                                */


/** @brief Enumerations for EDMA channels
*
*  There are 8 QDMA channels -
*
*/

#define    CSL_DAT_QCHA_0                   0 /**<   QDMA Channel 0           */
#define    CSL_DAT_QCHA_1                   1 /**<   QDMA Channel 1           */
#define    CSL_DAT_QCHA_2                   2 /**<   QDMA Channel 2           */
#define    CSL_DAT_QCHA_3                   3 /**<   QDMA Channel 3           */
#define    CSL_DAT_QCHA_4                   4 /**<   QDMA Channel 4           */
#define    CSL_DAT_QCHA_5                   5 /**<   QDMA Channel 5           */
#define    CSL_DAT_QCHA_6                   6 /**<   QDMA Channel 6           */
#define    CSL_DAT_QCHA_7                   7 /**<   QDMA Channel 7           */

/** @brief Enumerations for EDMA Event Queues
*
*  There are two Event Queues. Q0 is the highest priority and Q1 is the least
*  priority
*
*/
typedef enum {
    CSL_DAT_PRI_DEFAULT    =                0, /* Queue 0 is default          */
    CSL_DAT_PRI_0          =                0, /* Queue 0                     */
    CSL_DAT_PRI_1          =                1  /* Queue 1                     */
} CSL_DatPriority;

/** @brief Enumeration for EDMA Regions
*
*
*/

#define CSL_DAT_REGION_GLOBAL              -1         /* Global Region        */
#define CSL_DAT_REGION_0                    0         /* EDMA Region 0        */
#define CSL_DAT_REGION_1                    1         /* EDMA Region 1        */
#define CSL_DAT_REGION_2                    2         /* EDMA Region 2        */
#define CSL_DAT_REGION_3                    3         /* EDMA Region 3        */

/** @brief Enumeration for peripheral frequencies
*
*
*/

#define CSL_SYSCLK_1_FREQ                   (300000000)
#define CSL_SYSCLK_2_FREQ                   (CSL_SYSCLK_1_FREQ/2)
#define CSL_SYSCLK_3_FREQ                   (CSL_SYSCLK_1_FREQ/3)
#define CSL_SYSCLK_4_FREQ                   (CSL_SYSCLK_1_FREQ/4)

#define CSL_ASYNC_2_FREQ                    (24000000)

/** I2C      */
#define CSL_I2C_0_MODULE_FREQ               (CSL_ASYNC_2_FREQ)
#define CSL_I2C_1_MODULE_FREQ               (CSL_SYSCLK_4_FREQ)

/** LCDC     */
#define CSL_LCDC_0_MODULE_FREQ              (CSL_SYSCLK_2_FREQ)

/** SPI      */
#define CSL_SPI_0_MODULE_FREQ               (CSL_SYSCLK_2_FREQ)
#define CSL_SPI_1_MODULE_FREQ               (CSL_SYSCLK_2_FREQ)

/** UART     */
#define CSL_UART_0_MODULE_FREQ              (CSL_SYSCLK_2_FREQ)
#define CSL_UART_1_MODULE_FREQ              (CSL_SYSCLK_2_FREQ)
#define CSL_UART_2_MODULE_FREQ              (CSL_SYSCLK_2_FREQ)

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_OMAPL137_H_ */
