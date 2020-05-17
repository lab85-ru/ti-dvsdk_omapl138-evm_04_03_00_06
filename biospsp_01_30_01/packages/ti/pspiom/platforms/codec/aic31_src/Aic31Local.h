/*
 * Aic31Local.h
 *
 * Aic3106 Codec Driver internal header file
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

/*
 *  ======== ti/pspiom/platforms/codec/src/Aic31Local.h ========
 *
 */

#ifndef _AIC31LOCAL_H_
#define _AIC31LOCAL_H_

#include <std.h>
#include <ti/pspiom/platforms/codec/ICodec.h>
#include <ti/pspiom/platforms/codec/Aic31.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------- constants -------- */

#define Aic31_P0_REG0      (Uint8)(0)  /**< Page Select                              */
#define Aic31_P0_REG1      (Uint8)(1)  /**<  Software Reset                          */
#define Aic31_P0_REG2      (Uint8)(2)  /**<  Codec Sample Rate Select                */
#define Aic31_P0_REG3      (Uint8)(3)  /**<  PLL Programming A                       */
#define Aic31_P0_REG4      (Uint8)(4)  /**<  PLL Programming B                       */
#define Aic31_P0_REG5      (Uint8)(5)  /**<  PLL Programming C                       */
#define Aic31_P0_REG6      (Uint8)(6)  /**<  PLL Programming D                       */
#define Aic31_P0_REG7      (Uint8)(7)  /**<  Codec Datapath Setup                    */
#define Aic31_P0_REG8      (Uint8)(8)  /**<  Audio Serial Data Interface Control A   */
#define Aic31_P0_REG9      (Uint8)(9)  /**<  Audio Serial Data Interface Control B   */
#define Aic31_P0_REG10     (Uint8)(10) /**<  Audio Serial Data Interface Control C   */
#define Aic31_P0_REG11     (Uint8)(11) /**<  Audio Codec Overflow Flag               */
#define Aic31_P0_REG12     (Uint8)(12) /**<  Audio Codec Digital Filter Control      */
#define Aic31_P0_REG13     (Uint8)(13) /**<  Headset / Button Press Detection A      */
#define Aic31_P0_REG14     (Uint8)(14) /**<  Headset / Button Press Detection B      */
#define Aic31_P0_REG15     (Uint8)(15) /**<  Left ADC PGA Gain Control               */
#define Aic31_P0_REG16     (Uint8)(16) /**<  Right ADC PGA Gain Control              */
#define Aic31_P0_REG17     (Uint8)(17) /**<  MIC3L/R to Left ADC Control             */
#define Aic31_P0_REG18     (Uint8)(18) /**<  MIC3L/R to Right ADC Control            */
#define Aic31_P0_REG19     (Uint8)(19) /**<  LINE1L to Left ADC Control              */
#define Aic31_P0_REG20     (Uint8)(20) /**<  LINE2L to Left ADC Control              */
#define Aic31_P0_REG21     (Uint8)(21) /**<  LINE1R to Left ADC Control              */
#define Aic31_P0_REG22     (Uint8)(22) /**<  LINE1R to Right ADC Control             */
#define Aic31_P0_REG23     (Uint8)(23) /**<  LINE2R to Right ADC Control             */
#define Aic31_P0_REG24     (Uint8)(24) /**<  LINE1L to Right ADC Control             */
#define Aic31_P0_REG25     (Uint8)(25) /**<  MICBIAS Control                         */
#define Aic31_P0_REG26     (Uint8)(26) /**<  Left AGC Control A                      */
#define Aic31_P0_REG27     (Uint8)(27) /**<  Left AGC Control B                      */
#define Aic31_P0_REG28     (Uint8)(28) /**<  Left AGC Control C                      */
#define Aic31_P0_REG29     (Uint8)(29) /**<  Right AGC Control A                     */
#define Aic31_P0_REG30     (Uint8)(30) /**<  Right AGC Control B                     */
#define Aic31_P0_REG31     (Uint8)(31) /**<  Right AGC Control C                     */
#define Aic31_P0_REG32     (Uint8)(32) /**<  Left AGC Gain                           */
#define Aic31_P0_REG33     (Uint8)(33) /**<  Right AGC Gain                          */
#define Aic31_P0_REG34     (Uint8)(34) /**<  Left AGC Noise Gate Debounce            */
#define Aic31_P0_REG35     (Uint8)(35) /**<  Right AGC Noise Gate Debounce           */
#define Aic31_P0_REG36     (Uint8)(36) /**<  ADC Flag                                */
#define Aic31_P0_REG37     (Uint8)(37) /**<  DAC Power and Output Driver Control     */
#define Aic31_P0_REG38     (Uint8)(38) /**<  High Power Output Driver Control        */
#define Aic31_P0_REG40     (Uint8)(40) /**<  High Power Output Stage Control         */
#define Aic31_P0_REG41     (Uint8)(41) /**<  DAC Output Switching Control            */
#define Aic31_P0_REG42     (Uint8)(42) /**<  Output Driver Pop Reduction             */
#define Aic31_P0_REG43     (Uint8)(43) /**<  Left DAC Digital Volume Control         */
#define Aic31_P0_REG44     (Uint8)(44) /**<  Right DAC Digital Volume Control        */
#define Aic31_P0_REG45     (Uint8)(45) /**<  LINE2L to HPLOUT Volume Control         */
#define Aic31_P0_REG46     (Uint8)(46) /**<  PGA_L to HPLOUT Volume Control          */
#define Aic31_P0_REG47     (Uint8)(47) /**<  DAC_L1 to HPLOUT Volume Control         */
#define Aic31_P0_REG48     (Uint8)(48) /**<  LINE2R to HPLOUT Volume Control         */
#define Aic31_P0_REG49     (Uint8)(49) /**<  PGA_R to HPLOUT Volume Control          */
#define Aic31_P0_REG50     (Uint8)(50) /**<  DAC_R1 to HPLOUT Volume Control         */
#define Aic31_P0_REG51     (Uint8)(51) /**<  HPLOUT Output Level Control             */
#define Aic31_P0_REG52     (Uint8)(52) /**<  LINE2L to HPLCOM Volume Control         */
#define Aic31_P0_REG53     (Uint8)(53) /**<  PGA_L to HPLCOM Volume Control          */
#define Aic31_P0_REG54     (Uint8)(54) /**<  DAC_L1 to HPLCOM Volume Control         */
#define Aic31_P0_REG55     (Uint8)(55) /**<  LINE2R to HPLCOM Volume Control         */
#define Aic31_P0_REG56     (Uint8)(56) /**<  PGA_R to HPLCOM Volume Control          */
#define Aic31_P0_REG57     (Uint8)(57) /**<  DAC_R1 to HPLCOM Volume Control         */
#define Aic31_P0_REG58     (Uint8)(58) /**<  HPLCOM Output Level Control             */
#define Aic31_P0_REG59     (Uint8)(59) /**<  LINE2L to HPROUT Volume Control         */
#define Aic31_P0_REG60     (Uint8)(60) /**<  PGA_L to HPROUT Volume Control          */
#define Aic31_P0_REG61     (Uint8)(61) /**<  DAC_L1 to HPROUT Volume Control         */
#define Aic31_P0_REG62     (Uint8)(62) /**<  LINE2R to HPROUT Volume Control         */
#define Aic31_P0_REG63     (Uint8)(63) /**<  PGA_R to HPROUT Volume Control          */
#define Aic31_P0_REG64     (Uint8)(64) /**<  DAC_R1 to HPROUT Volume Control         */
#define Aic31_P0_REG65     (Uint8)(65) /**<  HPROUT Output Level Control             */
#define Aic31_P0_REG66     (Uint8)(66) /**<  LINE2L to HPRCOM Volume Control         */
#define Aic31_P0_REG67     (Uint8)(67) /**<  PGA_L to HPRCOM Volume Control          */
#define Aic31_P0_REG68     (Uint8)(68) /**<  DAC_L1 to HPRCOM Volume Control         */
#define Aic31_P0_REG69     (Uint8)(69) /**<  LINE2R to HPRCOM Volume Control         */
#define Aic31_P0_REG70     (Uint8)(70) /**<  PGA_R to HPRCOM Volume Control          */
#define Aic31_P0_REG71     (Uint8)(71) /**<  DAC_R1 to HPRCOM Volume Control         */
#define Aic31_P0_REG72     (Uint8)(72) /**<  HPRCOM Output Level Control             */
#define Aic31_P0_REG73     (Uint8)(73) /**<  LINE2L to MONO_LOP/M Volume Control     */
#define Aic31_P0_REG74     (Uint8)(74) /**<  PGA_L to MONO_LOP/M Volume Control      */
#define Aic31_P0_REG75     (Uint8)(75) /**<  DAC_L1 to MONO_LOP/M Volume Control     */
#define Aic31_P0_REG76     (Uint8)(76) /**<  LINE2R to MONO_LOP/M Volume Control     */
#define Aic31_P0_REG77     (Uint8)(77) /**<  PGA_R to MONO_LOP/M Volume Control      */
#define Aic31_P0_REG78     (Uint8)(78) /**<  DAC_R1 to MONO_LOP/M Volume Control     */
#define Aic31_P0_REG79     (Uint8)(79) /**<  MONO_LOP/M Output Level Control         */
#define Aic31_P0_REG80     (Uint8)(80) /**<  LINE2L to LEFT_LOP/M Volume Control     */
#define Aic31_P0_REG81     (Uint8)(81) /**<  PGA_L to LEFT_LOP/M Volume Control      */
#define Aic31_P0_REG82     (Uint8)(82) /**<  DAC_L1 to LEFT_LOP/M Volume Control     */
#define Aic31_P0_REG83     (Uint8)(83) /**<  LINE2R to LEFT_LOP/M Volume Control     */
#define Aic31_P0_REG84     (Uint8)(84) /**<  PGA_R to LEFT_LOP/M Volume Control      */
#define Aic31_P0_REG85     (Uint8)(85) /**<  DAC_R1 to LEFT_LOP/M Volume Control     */
#define Aic31_P0_REG86     (Uint8)(86) /**<  LEFT_LOP/M Output Level Control         */
#define Aic31_P0_REG87     (Uint8)(87) /**<  LINE2L to RIGHT_LOP/M Volume Control    */
#define Aic31_P0_REG88     (Uint8)(88) /**<  PGA_L to RIGHT_LOP/M Volume Control     */
#define Aic31_P0_REG89     (Uint8)(89) /**<  DAC_L1 to RIGHT_LOP/M Volume Control    */
#define Aic31_P0_REG90     (Uint8)(90) /**<  LINE2R to RIGHT_LOP/M Volume Control    */
#define Aic31_P0_REG91     (Uint8)(91) /**<  PGA_R to RIGHT_LOP/M Volume Control     */
#define Aic31_P0_REG92     (Uint8)(92) /**<  DAC_R1 to RIGHT_LOP/M Volume Control    */
#define Aic31_P0_REG93     (Uint8)(93) /**<  RIGHT_LOP/M Output Level Control        */
#define Aic31_P0_REG94     (Uint8)(94) /**<  Module Power Status                     */
#define Aic31_P0_REG95     (Uint8)(95) /**< O/P Driver Short Circuit Detection Status*/
#define Aic31_P0_REG96     (Uint8)(96) /**<  Sticky Interrupt Flags                  */
#define Aic31_P0_REG97     (Uint8)(97) /**<  Real-time Interrupt Flags               */
#define Aic31_P0_REG98     (Uint8)(98) /**<  GPIO1 Control                           */
#define Aic31_P0_REG99     (Uint8)(99) /**<  GPIO2 Control                           */
#define Aic31_P0_REG100    (Uint8)(100)  /**<  Additional GPIO Control A             */
#define Aic31_P0_REG101    (Uint8)(101)  /**<  Additional GPIO Control B             */
#define Aic31_P0_REG102    (Uint8)(102)  /**<  Clock Generation Control              */

#define Aic31_PAGE_SELECT_ADDR                      (Uint8)(0x0000)
#define Aic31_RESET_ADDR                            (Uint8)(0x0001)
#define Aic31_CODEC_SAMPLE_RATE_ADDR                (Uint8)(0x0002)
#define Aic31_PLLA_PROG_ADDR                        (Uint8)(0x0003)
#define Aic31_PLLB_PROG_ADDR                        (Uint8)(0x0004)
#define Aic31_PLLC_PROG_ADDR                        (Uint8)(0x0005)
#define Aic31_PLLD_PROG_ADDR                        (Uint8)(0x0006)
#define Aic31_CODEC_DATAPATH_SETUP_ADDR             (Uint8)(0x0007)
#define Aic31_SERIAL_INTERFACEA_ADDR                (Uint8)(0x0008)
#define Aic31_SERIAL_INTERFACEB_ADDR                (Uint8)(0x0009)
#define Aic31_SERIAL_INTERFACEC_ADDR                (Uint8)(0x000A)
#define Aic31_CODEC_OVERFLOW_FLAG_ADDR              (Uint8)(0x000B)
#define Aic31_CODEC_DIGIFILTER_CTRL_ADDR            (Uint8)(0x000C)
#define Aic31_CODEC_HEADSET_DETA_ADDR               (Uint8)(0x000D)
#define Aic31_CODEC_HEADSET_DETB_ADDR               (Uint8)(0x000E)
#define Aic31_LEFT_ADCPGA_GAIN_CTRL_ADDR            (Uint8)(0x000F)
#define Aic31_RIGHT_ADCPGA_GAIN_CTRL_ADDR           (Uint8)(0x0010)
#define Aic31_MIC3LR_TO_LEFTADC_CTRL_ADDR           (Uint8)(0x0011)
#define Aic31_MIC3LR_TO_RIGHTADC_CTRL_ADDR          (Uint8)(0x0012)
#define Aic31_LINE1L_TO_LEFT_ADC_CTRL_ADDR          (Uint8)(0x0013)
#define Aic31_LINE2L_TO_LEFT_ADC_CTRL_ADDR          (Uint8)(0x0014)
#define Aic31_LINE1R_TO_LEFT_ADC_CTRL_ADDR          (Uint8)(0x0015)
#define Aic31_LINE1R_TO_RIGHT_ADC_CTL_ADDR          (Uint8)(0x0016)
#define Aic31_LINE2R_TO_RIGTH_ADC_CTRL_ADDR         (Uint8)(0x0017)
#define Aic31_LINE1L_TO_RIGTH_ADC_CTRL_ADDR         (Uint8)(0x0018)
#define Aic31_MICBIAS_CTRL_ADDR                     (Uint8)(0x0019)
#define Aic31_LEFT_AGC_CTRLA_ADDR                   (Uint8)(0x001A)
#define Aic31_LEFT_AGC_CTRLB_ADDR                   (Uint8)(0x001B)
#define Aic31_LEFT_AGC_CTRLC_ADDR                   (Uint8)(0x001C)
#define Aic31_RIGHT_AGC_CTRLA_ADDR                  (Uint8)(0x001D)
#define Aic31_RIGHT_AGC_CTRLB_ADDR                  (Uint8)(0x001E)
#define Aic31_RIGHT_AGC_CTRLC_ADDR                  (Uint8)(0x001F)
#define Aic31_LEFT_AGC_GAIN_ADDR                    (Uint8)(0x0020)
#define Aic31_RIGHT_AGC_GAIN_ADDR                   (Uint8)(0x0021)
#define Aic31_LEFT_AGC_NOISE_GATE_DEBOUNCE_ADDR     (Uint8)(0x0022)
#define Aic31_RIGHT_AGC_NOISE_GATE_DEBOUNCE_ADDR    (Uint8)(0x0023)
#define Aic31_ADC_FLAG_ADDR                         (Uint8)(0x0024)
#define Aic31_DAC_POWER_OUTPUT_DRIVER_CTRL_ADDR     (Uint8)(0x0025)
#define Aic31_HP_OUTPUT_DRIVER_CTRL_ADDR            (Uint8)(0x0026)
#define Aic31_RESERVE                               (Uint8)(0x0027)
#define Aic31_HP_OUTPUT_STAGE_CTRL_ADDR             (Uint8)(0x0028)
#define Aic31_DAC_OUTPUT_SWITCH_CTRL_ADDR           (Uint8)(0x0029)
#define Aic31_OUTPUT_DRV_POP_REDUCTION_ADDR         (Uint8)(0x002A)
#define Aic31_LEFT_DAC_VOL_CTRL_ADDR                (Uint8)(0x002B)
#define Aic31_RIGHT_DAC_VOL_CTRL_ADDR               (Uint8)(0x002C)
#define Aic31_LINE2L_TO_HPLOUT_VOL_CTRL_ADDR        (Uint8)(0x002D)
#define Aic31_PGA_L_TO_HPLOUT_VOL_CTRL_ADDR         (Uint8)(0x002E)
#define Aic31_DAC_L1_TO_HPLOUT_VOL_CTRL_ADDR        (Uint8)(0x002F)
#define Aic31_LINE2R_TO_HPLOUT_VOL_CTRL_ADDR        (Uint8)(0x0030)
#define Aic31_PGA_R_TO_HPLOUT_VOL_CTRL_ADDR         (Uint8)(0x0031)
#define Aic31_DAC_R1_TO_HPLOUT_VOL_CTRL_ADDR        (Uint8)(0x0032)
#define Aic31_HPLOUT_OUTPUT_LEVEL_CTRL_ADDR         (Uint8)(0x0033)
#define Aic31_LIN2L_TO_HPLCOM_VOL_CTRL_ADDR         (Uint8)(0x0034)
#define Aic31_PGA_L_TO_HPLCOM_VOL_CTRL_ADDR         (Uint8)(0x0035)
#define Aic31_DAC_L1_TO_HPLCOM_VOL_CTRL_ADDR        (Uint8)(0x0036)
#define Aic31_LINE2R_TO_HPLCOM_VOL_CTRL_ADDR        (Uint8)(0x0037)
#define Aic31_PGA_R_TO_HPLCOM_VOL_CTRL_ADDR         (Uint8)(0x0038)
#define Aic31_DAC_R1_TO_HPLCOM_VOL_CTRL_ADDR        (Uint8)(0x0039)
#define Aic31_HPLCOM_OUTPUT_LEVEL_CTRL_ADDR         (Uint8)(0x003A)
#define Aic31_LINE2L_TO_HPROUT_VOL_CTRL_ADDR        (Uint8)(0x003B)
#define Aic31_PGA_L_TO_HPROUT_VOL_CTRL_ADDR         (Uint8)(0x003C)
#define Aic31_DAC_L1_TO_HPROUT_VOL_CTRL_ADDR        (Uint8)(0x003D)
#define Aic31_LINE2R_TO_HPROUT_VOL_CTRL_ADDR        (Uint8)(0x003E)
#define Aic31_PGA_R_TO_HPROUT_VOL_CTRL_ADDR         (Uint8)(0x003F)
#define Aic31_DAC_R1_TO_HPROUT_VOL_CTRL_ADDR        (Uint8)(0x0040)
#define Aic31_HPROUT_OUTPUT_LEVEL_CTRL_ADDR         (Uint8)(0x0041)
#define Aic31_LIN2L_TO_HPRCOM_VOL_CTRL_ADDR         (Uint8)(0x0042)
#define Aic31_PGA_L_TO_HPRCOM_VOL_CTRL_ADDR         (Uint8)(0x0043)
#define Aic31_DAC_L1_TO_HPRCOM_VOL_CTRL_ADDR        (Uint8)(0x0044)
#define Aic31_LINE2R_TO_HPRCOM_VOL_CTRL_ADDR        (Uint8)(0x0045)
#define Aic31_PGA_R_TO_HPRCOM_VOL_CTRL_ADDR         (Uint8)(0x0046)
#define Aic31_DAC_R1_TO_HPRCOM_VOL_CTRL_ADDR        (Uint8)(0x0047)
#define Aic31_HPRCOM_OUTPUT_LEVEL_CTRL_ADDR         (Uint8)(0x0048)
#define Aic31_LIN2L_TO_MONO_LOPM_VOL_CTRL_ADDR      (Uint8)(0x0049)
#define Aic31_PGA_L_TO_MONO_LOPM_VOL_CTRL_ADDR      (Uint8)(0x004A)
#define Aic31_DAC_L1_TO_MONO_LOPM_VOL_CTRL_ADDR     (Uint8)(0x004B)
#define Aic31_LINE2R_TO_MONO_LOPM_VOL_CTRL_ADDR     (Uint8)(0x004C)
#define Aic31_PGA_R_TO_MONO_LOPM_VOL_CTRL_ADDR      (Uint8)(0x004D)
#define Aic31_DAC_R1_TO_MONO_LOPM_VOL_CTRL_ADDR     (Uint8)(0x004E)
#define Aic31_MONO_LOPM_OUTPUT_LEVEL_CTRL_ADDR      (Uint8)(0x004F)
#define Aic31_LIN2L_TO_LEFT_LOPM_VOL_CTRL_ADDR      (Uint8)(0x0050)
#define Aic31_PGA_L_TO_LEFT_LOPM_VOL_CTRL_ADDR      (Uint8)(0x0051)
#define Aic31_DAC_L1_TO_LEFT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x0052)
#define Aic31_LINE2R_TO_LEFT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x0053)
#define Aic31_PGA_R_TO_LEFT_LOPM_VOL_CTRL_ADDR      (Uint8)(0x0054)
#define Aic31_DAC_R1_TO_LEFT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x0055)
#define Aic31_LEFT_LOPM_OUTPUT_LEVEL_CTRL_ADDR      (Uint8)(0x0056)
#define Aic31_LIN2L_TO_RIGHT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x0057)
#define Aic31_PGA_L_TO_RIGHT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x0058)
#define Aic31_DAC_L1_TO_RIGHT_LOPM_VOL_CTRL_ADDR    (Uint8)(0x0059)
#define Aic31_LINE2R_TO_RIGHT_LOPM_VOL_CTRL_ADDR    (Uint8)(0x005A)
#define Aic31_PGA_R_TO_RIGHT_LOPM_VOL_CTRL_ADDR     (Uint8)(0x005B)
#define Aic31_DAC_R1_TO_RIGHT_LOPM_VOL_CTRL_ADDR    (Uint8)(0x005C)
#define Aic31_RIGHT_LOPM_OUTPUT_LEVEL_CTRL_ADDR     (Uint8)(0x005D)
#define Aic31_MODULE_POWER_STAT_ADDR                (Uint8)(0x005E)
#define Aic31_OUTPUT_DRV_SHORTCKT_DET_STAT_ADDR     (Uint8)(0x005F)
#define Aic31_STICKY_INTR_FLAGS_ADDR                (Uint8)(0x0060)
#define Aic31_REALTIME_INTR_FLAGS_ADDR              (Uint8)(0x0061)
#define Aic31_GPIO1_CTRL_ADDR                       (Uint8)(0x0062)
#define Aic31_GPIO2_CTRL_ADDR                       (Uint8)(0x0063)
#define Aic31_ADDITIONAL_GPIOCTRLA_ADDR             (Uint8)(0x0064)
#define Aic31_ADDITIONAL_GPIOCTRLB_ADDR             (Uint8)(0x0065)
#define Aic31_CLKGEN_CTRL_ADDR                      (Uint8)(0x0066)




/* Selct Aic31 page 0 registers                   */
#define Aic31_PAGE_0              (Uint8)(0x00)

/* Max value of gain to be programmed to the DAC  */
#define Aic31_MAX_DAC_GAIN_VALUE  (Uint8)(0x7F)

/* Max value of gain to be programmed to the AGC  */
#define Aic31_MAX_AGC_GAIN_VALUE  (Uint8)(0x77)

/* Max value of gain to be programmed to the PGA  */
#define Aic31_MAX_PGA_GAIN_VALUE  (Uint8)(0x77)

/* MAX percent of the gain allowed                */
#define Aic31_MAX_GAIN_PERCENT    (Uint8)(100u)


#define Aic31_REG_READ_PASS       (Bool)0

#define Aic31_REG_READ_FAIL       (Bool)1

#define Aic31_REG_WRITE_PASS      (Bool)0

#define Aic31_REG_WRITE_FAIL      (Bool)1


/* -------- structs -------- */

/**
 *  \brief Channel Object which stores the channel related information.
 *
 *  This structure defines the audio codec channel specific data.it defines
 *  the state of the channel and stores the information required by the
 *  channel.
 */
typedef struct Aic31_Channel_Object
{
    ICodec_DriverState  chanStatus;
    /**< The state of the channel. It could be opened or closed.              */

    ICodec_Channel      channelMode;
    /**< The channe mode. It could be transmit or recieve                     */

    Ptr                 devHandle;
    /**< Pointer to the audio codec object                                    */

    Uint32              samplingRate;
    /**< Current sampling rate of the channel                                 */

    Uint32              chanGain;
    /**< The current gain of the channel in percentage                        */
}Aic31_Channel_Object;

/**
 *  \brief Hardware information structure
 *
 *  Aic31 structure containing the instance specific data.This data is
 *  loaded from the platform.xs file(this file contains the information
 *  specific to the platform).
 */
typedef struct Aic31_HwInfo {
    Uint32 deviceAddress;
}Aic31_HwInfo;

/* -------- module-wide state -------- */

/**
 *  \brief Moule state object
 */
typedef struct {
    Bool *inUse;
} Aic31_Module_State;

/* -------- per-instance state -------- */

/**
 *  \brief Instance State object
 */
typedef struct
{
    Uint8                   instNum;
    /**< Instance number of the current codec type to be opened               */

    ICodec_DriverState      devState;
    /**< The state of the audio codec. It could created or deleted state      */

    ICodec_CodecType        acType;
    /**< Identifies the type of audio codec (AIC12 / AIC23 / TP)              */

    ICodec_ControlBusType   acControlBusType;
    /**< Type of the control bus (SPI or I2C)                                 */

    String                  acCtrlBusName;
    /**< Instance number of the control bus  (I2C or SPI)                     */

    DEV_Device             *acCtrlBusHandle;
    /* pointer to the driver object of the control bus                        */

    Uint32                  acCodecId;
    /**< I2C slave address or SPI CS number                                   */

    ICodec_OpMode           acOpMode;
    /**< Audio Codec operating mode                                           */

    ICodec_DataType         acSerialDataType;
    /**< Audio Codec serial data type                                         */

    Uint32                  acSlotWidth;
    /**< Slot width of each channel                                           */

    ICodec_DataPath         acDataPath;
    /**< Data paths (Tx / Rx / Both Tx and Rx )                               */

    Bool                    isRxTxClockIndependent;
    /**< Defines if the serial clock is common for both Tx and Rx data paths  */

    Aic31_Channel_Object    ChanObj[ICodec_Channel_MAX];
    /**< The audio codec channel objects of transmit and receive channels     */

    Ptr                     hCtrlBus;
    /**< Handle of the control bus instance created and opened                */

    SEM_Obj                 semObj;
    /**< Semaphore Handle used while doing I2C transfer                       */

} Aic31_Object;

#if defined(CHIP_C6747) || defined (CHIP_OMAPL137)
/* Evm6747 specific defines */
    #define AIC31_NUM_INSTANCES   3
    /**<    Num of instances                                                  */
    #define AIC31_INST0_ADDRESS   0x16
    /**<    Address of first instance                                         */
    #define AIC31_INST1_ADDRESS   0x17
    /**<    Address of second instance                                        */
    #define AIC31_INST2_ADDRESS   0x18
    /**<    Address of third instance                                         */
#elif defined(CHIP_C6748) || defined (CHIP_OMAPL138)
    #define AIC31_NUM_INSTANCES   1
    /**<    Num of instances                                                  */
    #define AIC31_INST0_ADDRESS   0x18
    /**<    Address of first instance                                         */
#endif
#ifdef __cplusplus
}
#endif

#endif  /*_AIC31LOCAL_H_ */
