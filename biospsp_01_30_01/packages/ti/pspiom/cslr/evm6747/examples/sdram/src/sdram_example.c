/*
 * sdram_example.c
 *
 * This file contains the test / demo code to demonstrate basic SDRAM 
 * read/write operations using the Regsiter CSL macros.
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

/** ============================================================================
 *   @brief A simple example to demonstrate CSL  EMIFB register layer usage.
 *
 *   @file  sdram_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   Sdram example write-read verification for demonstrating CSL layer usage.
 *   The SDRAM is connected via the EMIFB peripheral and this example demostrates
 *   the use of the EMIFB CSL macros in intializing the EMIFB and verifying the
 *   same by writing different patterns to the SDRAM and reading it back to
 *   compare and display the result.
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Configure the CCS setup to work with the emulator being used
 *   2. Please refer CCS manual for setup configuration and loading
 *      proper GEL file
 *   3. Launch CCS window
 *   4. Open project sdram_example.pjt
 *   5. Build the project and load the .out file of the project.
 *   @endverbatim
 *
 * =============================================================================
 **/

#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/soc_C6747.h>
#include <ti/pspiom/cslr/cslr_emifb.h>
#include <ti/pspiom/cslr/cslr_syscfg_C6747.h>
#include <ti/pspiom/cslr/cslr_psc_C6747.h>
#include <stdio.h>


CSL_SyscfgRegsOvly  sysRegs     = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
CSL_PscRegsOvly    psc1Regs    = (CSL_PscRegsOvly)CSL_PSC_1_REGS;
CSL_EmifbRegsOvly    emifbRegs = (CSL_EmifbRegsOvly)CSL_EMIFB_0_REGS;

/* DDR Base address */
Uint32 ddr_base;
/* DDR size */
Uint32 ddr_size;

/* Function to test SDRAM read write */
int sdramTest( void );

/* Function to initialize EMIFB */
int emifbInit( void );

/* Function to configure SDRAM */
int configSdram( void );

/* Function for testing invalid SDRAM address range */
Uint32 meminvaddr32( Uint32 , Uint32 );

/* Function for testing valid SDRAM address range */
Uint32 memaddr32( Uint32 , Uint32 );

/* Function for filling an SDRAM address range */
Uint32 memfill32( Uint32 , Uint32 , Uint32 );


int main( void )
{
  int result = 0;

  /* Intialize EMIF */
  result = emifbInit();
  if(result < 0)
  {
      printf("EMIFB Initialization failed\n");
      return result;
  }
  else
  {
      printf("EMIFB Initialization success\n");
  }

  /* Configure SDRAM */
  result = configSdram();
  if(result < 0)
  {
      printf("SDRAM Configuration test failed\n");
      return result;
  }
  else
  {
      printf("SDRAM Configuration success\n");
  }

  /* Run SDRAM write/read test */
  result = sdramTest();
  if(result != 0)
  {
      printf("SDRAM Read/Write example test failed\n");
      return result;
  }
  else
  {
      printf("SDRAM Read/Write example test success\n");
  }

  return 0;
}


int emifbInit( void )
{
    volatile int pscTimeoutCount = 10240;
    int result = 0;

    sysRegs->KICK0R = 0x83e70b13;  // Kick0 register + data (unlock)
    sysRegs->KICK1R = 0x95a4f1e0;  // Kick1 register + data (unlock)

    /* Set PINMUX's for enabling EMIFB */

    //0x11111188 : EMIFB, Check EMU0/RTCK : TND Verify 15_12 bits
    sysRegs->PINMUX0  = ( CSL_SYSCFG_PINMUX0_PINMUX0_31_28_EMB_WE    << CSL_SYSCFG_PINMUX0_PINMUX0_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX0_PINMUX0_27_24_EMB_RAS   << CSL_SYSCFG_PINMUX0_PINMUX0_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX0_PINMUX0_23_20_EMB_CAS   << CSL_SYSCFG_PINMUX0_PINMUX0_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX0_PINMUX0_19_16_EMB_CS0   << CSL_SYSCFG_PINMUX0_PINMUX0_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX0_PINMUX0_15_12_RESERVED1 << CSL_SYSCFG_PINMUX0_PINMUX0_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX0_PINMUX0_11_8_EMB_SDCKE  << CSL_SYSCFG_PINMUX0_PINMUX0_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX0_PINMUX0_7_4_EMU0        << CSL_SYSCFG_PINMUX0_PINMUX0_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX0_PINMUX0_3_0_RTCK        << CSL_SYSCFG_PINMUX0_PINMUX0_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX1  = ( CSL_SYSCFG_PINMUX1_PINMUX1_31_28_EMB_A5   << CSL_SYSCFG_PINMUX1_PINMUX1_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX1_PINMUX1_27_24_EMB_A4   << CSL_SYSCFG_PINMUX1_PINMUX1_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX1_PINMUX1_23_20_EMB_A3   << CSL_SYSCFG_PINMUX1_PINMUX1_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX1_PINMUX1_19_16_EMB_A2   << CSL_SYSCFG_PINMUX1_PINMUX1_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX1_PINMUX1_15_12_EMB_A1   << CSL_SYSCFG_PINMUX1_PINMUX1_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX1_PINMUX1_11_8_EMB_A0    << CSL_SYSCFG_PINMUX1_PINMUX1_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX1_PINMUX1_7_4_EMB_BA0    << CSL_SYSCFG_PINMUX1_PINMUX1_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX1_PINMUX1_3_0_EMB_BA1    << CSL_SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX2  = ( CSL_SYSCFG_PINMUX2_PINMUX2_31_28_EMB_D31 << CSL_SYSCFG_PINMUX2_PINMUX2_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX2_PINMUX2_27_24_EMB_A12 << CSL_SYSCFG_PINMUX2_PINMUX2_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX2_PINMUX2_23_20_EMB_A11 << CSL_SYSCFG_PINMUX2_PINMUX2_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX2_PINMUX2_19_16_EMB_A10 << CSL_SYSCFG_PINMUX2_PINMUX2_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX2_PINMUX2_15_12_EMB_A9  << CSL_SYSCFG_PINMUX2_PINMUX2_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX2_PINMUX2_11_8_EMB_A8   << CSL_SYSCFG_PINMUX2_PINMUX2_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX2_PINMUX2_7_4_EMB_A7    << CSL_SYSCFG_PINMUX2_PINMUX2_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX2_PINMUX2_3_0_EMB_A6    << CSL_SYSCFG_PINMUX2_PINMUX2_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX3  = ( CSL_SYSCFG_PINMUX3_PINMUX3_31_28_EMB_D23 << CSL_SYSCFG_PINMUX3_PINMUX3_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX3_PINMUX3_27_24_EMB_D24 << CSL_SYSCFG_PINMUX3_PINMUX3_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX3_PINMUX3_23_20_EMB_D25 << CSL_SYSCFG_PINMUX3_PINMUX3_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX3_PINMUX3_19_16_EMB_D26 << CSL_SYSCFG_PINMUX3_PINMUX3_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX3_PINMUX3_15_12_EMB_D27 << CSL_SYSCFG_PINMUX3_PINMUX3_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX3_PINMUX3_11_8_EMB_D28  << CSL_SYSCFG_PINMUX3_PINMUX3_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX3_PINMUX3_7_4_EMB_D29   << CSL_SYSCFG_PINMUX3_PINMUX3_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX3_PINMUX3_3_0_EMB_D30   << CSL_SYSCFG_PINMUX3_PINMUX3_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX4  = ( CSL_SYSCFG_PINMUX4_PINMUX4_31_28_EMB_WE_DQM3  << CSL_SYSCFG_PINMUX4_PINMUX4_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX4_PINMUX4_27_24_EMB_D16      << CSL_SYSCFG_PINMUX4_PINMUX4_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX4_PINMUX4_23_20_EMB_D17      << CSL_SYSCFG_PINMUX4_PINMUX4_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX4_PINMUX4_19_16_EMB_D18      << CSL_SYSCFG_PINMUX4_PINMUX4_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX4_PINMUX4_15_12_EMB_D19      << CSL_SYSCFG_PINMUX4_PINMUX4_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX4_PINMUX4_11_8_EMB_D20       << CSL_SYSCFG_PINMUX4_PINMUX4_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX4_PINMUX4_7_4_EMB_D21        << CSL_SYSCFG_PINMUX4_PINMUX4_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX4_PINMUX4_3_0_EMB_D22        << CSL_SYSCFG_PINMUX4_PINMUX4_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX5  = ( CSL_SYSCFG_PINMUX5_PINMUX5_31_28_EMB_D6       << CSL_SYSCFG_PINMUX5_PINMUX5_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX5_PINMUX5_27_24_EMB_D5       << CSL_SYSCFG_PINMUX5_PINMUX5_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX5_PINMUX5_23_20_EMB_D4       << CSL_SYSCFG_PINMUX5_PINMUX5_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX5_PINMUX5_19_16_EMB_D3       << CSL_SYSCFG_PINMUX5_PINMUX5_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX5_PINMUX5_15_12_EMB_D2       << CSL_SYSCFG_PINMUX5_PINMUX5_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX5_PINMUX5_11_8_EMB_D1        << CSL_SYSCFG_PINMUX5_PINMUX5_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX5_PINMUX5_7_4_EMB_D0         << CSL_SYSCFG_PINMUX5_PINMUX5_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX5_PINMUX5_3_0_EMB_WE_DQM2    << CSL_SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT   );

    //0x11111111;  EMIFB
    sysRegs->PINMUX6  = ( CSL_SYSCFG_PINMUX6_PINMUX6_31_28_EMB_D14      << CSL_SYSCFG_PINMUX6_PINMUX6_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX6_PINMUX6_27_24_EMB_D13      << CSL_SYSCFG_PINMUX6_PINMUX6_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX6_PINMUX6_23_20_EMB_D12      << CSL_SYSCFG_PINMUX6_PINMUX6_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX6_PINMUX6_19_16_EMB_D11      << CSL_SYSCFG_PINMUX6_PINMUX6_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX6_PINMUX6_15_12_EMB_D10      << CSL_SYSCFG_PINMUX6_PINMUX6_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX6_PINMUX6_11_8_EMB_D9        << CSL_SYSCFG_PINMUX6_PINMUX6_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX6_PINMUX6_7_4_EMB_D8         << CSL_SYSCFG_PINMUX6_PINMUX6_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX6_PINMUX6_3_0_EMB_D7         << CSL_SYSCFG_PINMUX6_PINMUX6_3_0_SHIFT   );

    //0x11111111;  EMIFB, SPI0
    sysRegs->PINMUX7  = ( CSL_SYSCFG_PINMUX7_PINMUX7_31_28_SPI0_SCS0    << CSL_SYSCFG_PINMUX7_PINMUX7_31_28_SHIFT |
                          CSL_SYSCFG_PINMUX7_PINMUX7_27_24_SPI0_ENA     << CSL_SYSCFG_PINMUX7_PINMUX7_27_24_SHIFT |
                          CSL_SYSCFG_PINMUX7_PINMUX7_23_20_SPI0_CLK     << CSL_SYSCFG_PINMUX7_PINMUX7_23_20_SHIFT |
                          CSL_SYSCFG_PINMUX7_PINMUX7_19_16_SPI0_SIMO0   << CSL_SYSCFG_PINMUX7_PINMUX7_19_16_SHIFT |
                          CSL_SYSCFG_PINMUX7_PINMUX7_15_12_SPI0_SOMI0   << CSL_SYSCFG_PINMUX7_PINMUX7_15_12_SHIFT |
                          CSL_SYSCFG_PINMUX7_PINMUX7_11_8_EMB_WE_DQM0   << CSL_SYSCFG_PINMUX7_PINMUX7_11_8_SHIFT  |
                          CSL_SYSCFG_PINMUX7_PINMUX7_7_4_EMB_WE_DQM1    << CSL_SYSCFG_PINMUX7_PINMUX7_7_4_SHIFT   |
                          CSL_SYSCFG_PINMUX7_PINMUX7_3_0_EMB_D15        << CSL_SYSCFG_PINMUX7_PINMUX7_3_0_SHIFT   );


    /* Bring the EMIFB module out of reset */
    // deassert EMIFB local PSC reset and set NEXT state to ENABLE
    psc1Regs->MDCTL[CSL_PSC_EMIFB] = CSL_FMKT( PSC_MDCTL_NEXT, ENABLE )
                               | CSL_FMKT( PSC_MDCTL_LRST, DEASSERT );
    // move EMIFB PSC to Next state
    psc1Regs->PTCMD = CSL_FMKT(  PSC_PTCMD_GO0, SET );

    // wait for transition
    while( ( CSL_FEXT( psc1Regs->MDSTAT[CSL_PSC_EMIFB], PSC_MDSTAT_STATE )
             != CSL_PSC_MDSTAT_STATE_ENABLE )  && (pscTimeoutCount > 0) )
    {
        pscTimeoutCount--;
    }

    if(pscTimeoutCount == 0)
    {
        printf("EMIFB module power up timed out\n");
        result= -1;
    }

    return result;
}


int configSdram (void )
{
    volatile Uint32 temp = 0;

    // ISSI IS42S16160B-6BL SDRAM, 2 x 16M x 16 (32-bit data path), 133MHz
    temp = emifbRegs->SDCFG;
    temp =  ( ( CSL_EMIFB_SDCFG_TIMUNLOCK_SET << CSL_EMIFB_SDCFG_TIMUNLOCK_SHIFT)      |   // Unlock timing registers
               ( CSL_EMIFB_SDCFG_CL_TWO << CSL_EMIFB_SDCFG_CL_SHIFT )                     |   // CAS latency is 2
               ( CSL_EMIFB_SDCFG_IBANK_FOUR << CSL_EMIFB_SDCFG_IBANK_SHIFT )             |   // 4 bank SDRAM devices
               ( CSL_EMIFB_SDCFG_PAGESIZE_512W_PAGE << CSL_EMIFB_SDCFG_PAGESIZE_SHIFT ) );   // 512-word pages requiring 9 column address bits
    emifbRegs->SDCFG = temp;


    temp = emifbRegs->SDRFC;
    temp =  ( ( CSL_EMIFB_SDRFC_LP_MODE_LPMODE << CSL_EMIFB_SDRFC_LP_MODE_SHIFT)               |   // Low power mode disabled
              ( CSL_EMIFB_SDRFC_MCLKSTOP_EN_MCLKSTOP_DIS << CSL_EMIFB_SDRFC_MCLKSTOP_EN_SHIFT) |   // MCLK stoping disabled
              ( CSL_EMIFB_SDRFC_SR_PD_SELF_REFRESH << CSL_EMIFB_SDRFC_SR_PD_SHIFT)             |   // Selects self refresh instead of power down
              ( 1040 << CSL_EMIFB_SDRFC_REFRESH_RATE_SHIFT)                                               );   // Refresh rate = 7812.5ns / 7.5ns
    emifbRegs->SDRFC = temp;


    temp = emifbRegs->SDTIM1;
    temp =  ( ( 25 << CSL_EMIFB_SDTIM1_T_RFC_SHIFT )|   // (67.5ns / 7.55ns) - 1 = TRFC  @ 133MHz
              ( 2  << CSL_EMIFB_SDTIM1_T_RP_SHIFT  )|   // (20ns / 7.5ns) - 1 =TRP
              ( 2  << CSL_EMIFB_SDTIM1_T_RCD_SHIFT )|   // (20ns / 7.5ns) - 1 = TRCD
              ( 1  << CSL_EMIFB_SDTIM1_T_WR_SHIFT  )|   // (14ns / 7.5ns) - 1 = TWR
              ( 5  << CSL_EMIFB_SDTIM1_T_RAS_SHIFT )|   // (45ns / 7.5ns) - 1 = TRAS
              ( 8  << CSL_EMIFB_SDTIM1_T_RC_SHIFT  )|   // (67.5ns / 7.5ns) - 1 = TRC
              ( 2  << CSL_EMIFB_SDTIM1_T_RRD_SHIFT ) );   // *(((4 * 14ns) + (2 * 7.5ns)) / (4 * 7.5ns)) -1. = TRRD
                                                          // but it says to use this formula if 8 banks but only 4 are used here.
                                                          // and SDCFG1 register only suports upto 4 banks.
    emifbRegs->SDTIM1 = temp;


    temp = emifbRegs->SDTIM2;
    temp = ( (14 << CSL_EMIFB_SDTIM2_T_RAS_MAX_SHIFT) |   // not sure how they got this number. the datasheet says value should be
                                                          // "Maximum number of refresh_rate intervals from Activate to Precharge command"
                                                          // but has no equation. TRASMAX is 120k.
             ( 9 << CSL_EMIFB_SDTIM2_T_XSR_SHIFT)     |   // ( 70 / 7.5) - 1
             ( 5 << CSL_EMIFB_SDTIM2_T_CKE_SHIFT)     );        // ( 45 / 7.5 ) - 1
    emifbRegs->SDTIM2 = temp;


    temp = emifbRegs->SDCFG ;
    temp = ( ( CSL_EMIFB_SDCFG_SDREN_SDR_ENABLE << CSL_EMIFB_SDCFG_SDREN_SHIFT)             |
             ( CSL_EMIFB_SDCFG_TIMUNLOCK_CLEAR << CSL_EMIFB_SDCFG_TIMUNLOCK_SHIFT)       |    // lock timing registers
             ( CSL_EMIFB_SDCFG_CL_TWO << CSL_EMIFB_SDCFG_CL_SHIFT )                         |    // CAS latency is 2
             ( CSL_EMIFB_SDCFG_IBANK_FOUR << CSL_EMIFB_SDCFG_IBANK_SHIFT )                 |    // 4 bank SDRAM devices
             ( CSL_EMIFB_SDCFG_PAGESIZE_512W_PAGE << CSL_EMIFB_SDCFG_PAGESIZE_SHIFT )  );     // 512-word pages requiring 9 column address bits
    emifbRegs->SDCFG = temp;
    return 0;
}

int sdramTest( void )
{
        Int16 i, errors = 0;

    ddr_base = 0xc0004000;       // DDR memory
    ddr_size = 0x00010000;       // 1 MB

    printf( "  > Data test (quick)\n" );
    if ( memfill32( ddr_base, ddr_size, 0xFFFFFFFF ) )
        errors += 1;

    if ( memfill32( ddr_base, ddr_size, 0xAAAAAAAA ) )
        errors += 2;

    if ( memfill32( ddr_base, ddr_size, 0x55555555 ) )
        errors += 4;

    if ( memfill32( ddr_base, ddr_size, 0x00000000 ) )
        errors += 8;

    if ( errors )
        printf( "        > Error = 0x%x\n", errors );

#if(1)
    ddr_base = 0xc0004000;       // DDR memory
    ddr_size = 0x03FFC000;       // 63 MB+

    printf( "  > Addr test (quick)\n    " );
    for (i = 0; i < 11; i++)
    {
        printf("A%d ", i + 16);
        if ( memaddr32( ddr_base + (0x10000 << i), 0x10000 ) )
        {
            printf("(X) ");
            errors += 16;
        }
    }
    printf("\n");

    printf( "  > Inv addr test (quick)\n    " );
    for (i = 0; i < 11; i++)
    {
        printf("A%d ", i + 16);
        if ( meminvaddr32( ddr_base + (0x10000 << i), 0x10000 ) )
        {
            printf("(X) ");
            errors += 16;
        }
    }
    printf("\n");
#endif
    return errors;
}

Uint32 meminvaddr32( Uint32 start, Uint32 len )
{
    Uint32 i;
    Uint32 end = start + len;
    Uint32 errorcount = 0;
    Uint32 *pdata;

    /* Write Pattern */
    pdata = (Uint32 *)start;
    for ( i = start; i < end; i += 4 )
    {
        *pdata++ = ~i;
    }

    /* Read Pattern */
    pdata = (Uint32 *)start;
    for ( i = start; i < end; i += 4 )
    {
        if ( *pdata++ != ~i )
        {
            errorcount++;
            break;
        }
    }

    return errorcount;
}

Uint32 memaddr32( Uint32 start, Uint32 len )
{
    Uint32 i;
    Uint32 end = start + len;
    Uint32 errorcount = 0;
    Uint32 *pdata;


    /* Write Pattern */
    pdata = (Uint32 *)start;
    for ( i = start; i < end; i += 16 )
    {
        *pdata++ = i;
        *pdata++ = i + 4;
        *pdata++ = i + 8;
        *pdata++ = i + 12;
    }

    /* Read Pattern */
    pdata  = (Uint32 *)start;
    for ( i = start; i < end; i += 4 )
    {
        if ( *pdata++ != i )
        {
            errorcount++;
            break;
        }
    }

    return errorcount;
}

Uint32 memfill32( Uint32 start, Uint32 len, Uint32 val )
{
    Uint32 i;
    Uint32 end = start + len;
    Uint32 errorcount = 0;
    Uint32 *pdata;

    /* Write Pattern */
    pdata = (Uint32 *)start;
    for ( i = start; i < end; i += 4 )
    {
        *pdata++ = val;
    }

    /* Read Pattern */
    pdata = (Uint32 *)start;
    for ( i = start; i < end; i += 4 )
    {
        if ( *pdata++ != val )
        {
            errorcount++;
            break;
        }
    }

    return errorcount;
}
