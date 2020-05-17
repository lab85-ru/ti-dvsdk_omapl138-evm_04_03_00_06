/*
 * mddr_example.c
 *
 * This file contains the test / demo code to demonstrate basic mobile DDR 
 * read/writeoperations using the Regsiter CSL macros.
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
 *   @brief A simple example to demonstrate CSL  DDR2/MDDR register layer usage.
 *
 *   @file  mddr_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   Ddr2/Mddr example write-read verification for demonstrating CSL layer usage.
 *   This example demostrates the use of the DDR2/MDDR CSL macros in intializing
 *   the DDR2/MDDR and verifying the same by writing different patterns to the 
 *   DDR2/MDDR and reading it back to compare and display the result.
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Configure the CCS setup to work with the emulator being used
 *   2. Please refer CCS manual for setup configuration and loading
 *      proper GEL file
 *   3. Launch CCS window
 *   4. Open project mddr_example.pjt
 *   5. Build the project and load the .out file of the project.
 *   @endverbatim
 *
 * =============================================================================
 **/

#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/soc_C6748.h>
#include <ti/pspiom/cslr/cslr_ddr2_mddr.h>
#include <ti/pspiom/cslr/cslr_syscfg0_C6748.h>
#include <ti/pspiom/cslr/cslr_syscfg1_C6748.h>
#include <ti/pspiom/cslr/cslr_psc_C6748.h>
#include <stdio.h>


CSL_Ddr2_MddrRegsOvly   ddrRegs   = (CSL_Ddr2_MddrRegsOvly)CSL_DDR2_0_CTRL_REGS;
CSL_SyscfgRegsOvly      sys0Regs  = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
CSL_Syscfg1RegsOvly     sys1Regs  = (CSL_Syscfg1RegsOvly)CSL_SYSCFG_1_REGS;
CSL_PscRegsOvly         psc1Regs  = (CSL_PscRegsOvly)CSL_PSC_1_REGS;


/* DDR Base address */
Uint32 ddr_base;
/* DDR size */
Uint32 ddr_size;

/* Function to test SDRAM read write */
int ddrTest( void );

/* Function to initialize DDR2/MDDR */
int ddrInit( void );

/* Function to configure SDRAM */
int configDdr( void );

/* Function for testing invalid SDRAM address range */
Uint32 meminvaddr32( Uint32 , Uint32 );

/* Function for testing valid SDRAM address range */
Uint32 memaddr32( Uint32 , Uint32 );

/* Function for filling an SDRAM address range */
Uint32 memfill32( Uint32 , Uint32 , Uint32 );

/* Generic delay function */
void delay(Uint32 _delayCount);

int main( void )
{
    int result = 0;
    
    /* Intialize EMIF */
    result = ddrInit();
    if(result < 0)
    {
        printf("ddrInit Initialization failed\n");
        return result;
    }
    else
    {
        printf("ddrInit Initialization success\n");
    }
    
    /* Configure SDRAM */
    result = configDdr();
    if(result < 0)
    {
        printf("DDR Configuration test failed\n");
        return result;
    }
    else
    {
        printf("DDR Configuration success\n");
    }
    
    /* Run SDRAM write/read test */
    result = ddrTest();
    if(result != 0)
    {
        printf("DDR Read/Write example test failed\n");
        return result;
    }
    else
    {
        printf("DDR Read/Write example test success\n");
    }
    
    return 0;
}


int ddrInit( void )
{
    volatile int timeoutCount = 10240;
    int result = 0;

    sys0Regs->KICK0R = 0x83e70b13;  // Kick0 register + data (unlock)
    sys0Regs->KICK1R = 0x95a4f1e0;  // Kick1 register + data (unlock)

    sys0Regs->CFGCHIP3 |= ((0 << 7) & 0x00000080);

    /* Bring the DDR2/MDDR module out of reset */
    // deassert local PSC reset and set NEXT state to ENABLE
    psc1Regs->MDCTL[CSL_PSC_DDR2_MDDR] = CSL_FMKT( PSC_MDCTL_NEXT, ENABLE )
                               | CSL_FMKT( PSC_MDCTL_LRST, DEASSERT );
    // move EMIFB PSC to Next state
    psc1Regs->PTCMD = CSL_FMKT(  PSC_PTCMD_GO0, SET );

    // wait for transition
    while( ( CSL_FEXT( psc1Regs->MDSTAT[CSL_PSC_DDR2_MDDR], PSC_MDSTAT_STATE )
             != CSL_PSC_MDSTAT_STATE_ENABLE )  && (timeoutCount > 0) )
    {
        timeoutCount--;
    }

    if(timeoutCount == 0)
    {
        printf("EMIFB module power up timed out\n");
        result= -1;
    }

    if(0 == result)
    {
        timeoutCount = 100;
        
        /* If power down bit is set then enable it                            */
        if(CSL_SYSCFG1_VTPIO_CTL_POWERDN_MASK == \
           (sys1Regs->VTPIO_CTL & CSL_SYSCFG1_VTPIO_CTL_POWERDN_MASK))
        {
            /* Enable power down                                              */
            sys1Regs->VTPIO_CTL |= (CSL_SYSCFG1_VTPIO_CTL_IOPWRDN_MASK);
            
            /* Enable power up                                                */
            sys1Regs->VTPIO_CTL &= ~(CSL_SYSCFG1_VTPIO_CTL_POWERDN_MASK);
            
            /* Pulse the CLKRZ bit to enable caliberation                     */
            sys1Regs->VTPIO_CTL |= (CSL_SYSCFG1_VTPIO_CTL_CLKRZ_MASK);/* Set  */
            sys1Regs->VTPIO_CTL &= ~(CSL_SYSCFG1_VTPIO_CTL_CLKRZ_MASK);/*Clear*/
            delay(10);/* CLKRZ should be low atleast for 2ns                  */
            sys1Regs->VTPIO_CTL |= (CSL_SYSCFG1_VTPIO_CTL_CLKRZ_MASK); /*Set  */
            
            /* Polling READY bit to see when VTP calibration is done          */
            while((CSL_SYSCFG1_VTPIO_CTL_READY_MASK != 
                   (sys1Regs->VTPIO_CTL & CSL_SYSCFG1_VTPIO_CTL_READY_MASK))
                   && (timeoutCount > 0))
            {
                printf("\nWaiting for VTP to be ready");
                delay(100);
                timeoutCount--;
            }
        
            if(timeoutCount == 0)
            {
                printf("\nFailed to complete VTP calibration");
                result= -1;
            }
            
            if(0 == result)
            {
                /* Set LOCK bit for static mode                               */
                sys1Regs->VTPIO_CTL |= CSL_SYSCFG1_VTPIO_CTL_LOCK_MASK;       
                /* Set PWRSAVE bit to save power                              */
                sys1Regs->VTPIO_CTL |= CSL_SYSCFG1_VTPIO_CTL_PWRSAVE_MASK;    
            } 
        }
    }
    
    return result;
}


int configDdr (void )
{
 
    /* Read latency                                                           */
    ddrRegs->DRPYC1R =  /*0xC4;*/((4 << CSL_DDR2_MDDR_DRPYC1R_RL_SHIFT)                              |
                        (CSL_DDR2_MDDR_DRPYC1R_PWRDNEN_PWREN << CSL_DDR2_MDDR_DRPYC1R_PWRDNEN_SHIFT) |
                        (CSL_DDR2_MDDR_DRPYC1R_EXT_STRBEN_EXT_STRB << CSL_DDR2_MDDR_DRPYC1R_EXT_STRBEN_SHIFT));
  
    /* Pagesize = 2*/
    ddrRegs->SDCR = ((CSL_DDR2_MDDR_SDCR_PAGESIZE_1024WORD << CSL_DDR2_MDDR_SDCR_PAGESIZE_SHIFT)       |
                     (CSL_DDR2_MDDR_SDCR_IBANK_FOUR << CSL_DDR2_MDDR_SDCR_IBANK_SHIFT)                 |
                     (CSL_DDR2_MDDR_SDCR_CL_THREE << CSL_DDR2_MDDR_SDCR_CL_SHIFT)                      |
                     (CSL_DDR2_MDDR_SDCR_NM_16BIT << CSL_DDR2_MDDR_SDCR_NM_SHIFT)                      |
                     (CSL_DDR2_MDDR_SDCR_TIMUNLOCK_SET << CSL_DDR2_MDDR_SDCR_TIMUNLOCK_SHIFT)          |
                     (CSL_DDR2_MDDR_SDCR_SDRAMEN_SDR_EN << CSL_DDR2_MDDR_SDCR_SDRAMEN_SHIFT)           |
                     (CSL_DDR2_MDDR_SDCR_DDREN_DDR_EN << CSL_DDR2_MDDR_SDCR_DDREN_SHIFT)               |
                     (CSL_DDR2_MDDR_SDCR_DDR2EN_DDR2_EN << CSL_DDR2_MDDR_SDCR_DDR2EN_SHIFT)            |
                     (CSL_DDR2_MDDR_SDCR_BOOTUNLOCK_CHANGE << CSL_DDR2_MDDR_SDCR_BOOTUNLOCK_SHIFT)     |
                     (CSL_DDR2_MDDR_SDCR_IBANK_POS_DDR_ADDR_SCHM << CSL_DDR2_MDDR_SDCR_IBANK_POS_SHIFT));
                
    /*  Enable DDR  */
    ddrRegs->SDCR = ((ddrRegs->SDCR & ~CSL_DDR2_MDDR_SDCR_DDR2EN_MASK)                 | 
                      (CSL_DDR2_MDDR_SDCR_DDR2EN_DDR2_EN << CSL_DDR2_MDDR_SDCR_DDR2EN_SHIFT));
                      
    /* Enable MDDR  */
    ddrRegs->SDCR = ((ddrRegs->SDCR & (~CSL_DDR2_MDDR_SDCR_BOOTUNLOCK_MASK & ~CSL_DDR2_MDDR_SDCR_DDR2EN_MASK )) |
                     (CSL_DDR2_MDDR_SDCR_MSDRAMEN_MSDR_EN << CSL_DDR2_MDDR_SDCR_MSDRAMEN_SHIFT)); 
                
    
    /* Configure timing                                                       */
    ddrRegs->SDTIMR1 = ((1 << CSL_DDR2_MDDR_SDTIMR1_T_WTR_SHIFT)    |
                        (1 << CSL_DDR2_MDDR_SDTIMR1_T_RRD_SHIFT)    |
                        (10 << CSL_DDR2_MDDR_SDTIMR1_T_RC_SHIFT)    |
                        (7 << CSL_DDR2_MDDR_SDTIMR1_T_RAS_SHIFT)    |
                        (2 << CSL_DDR2_MDDR_SDTIMR1_T_WR_SHIFT)     |
                        (2 << CSL_DDR2_MDDR_SDTIMR1_T_RCD_SHIFT)    |
                        (2 << CSL_DDR2_MDDR_SDTIMR1_T_RP_SHIFT)     |
                        (16 << CSL_DDR2_MDDR_SDTIMR1_T_RFC_SHIFT));
                        
    ddrRegs->SDTIMR2 = ((0 << CSL_DDR2_MDDR_SDTIMR2_T_CKE_SHIFT)        |
                        (1 << CSL_DDR2_MDDR_SDTIMR2_T_RTP_SHIFT)        |
                        (199 << CSL_DDR2_MDDR_SDTIMR2_T_XSRD_SHIFT)     |
                        (21 << CSL_DDR2_MDDR_SDTIMR2_T_XSNR_SHIFT));
                        
    /* Lock timer control registers                                           */
    ddrRegs->SDCR   &= (~CSL_DDR2_MDDR_SDCR_TIMUNLOCK_MASK);
    
    /* 4 banks refresh and 9 rows                                             */
    ddrRegs->SDCR2 = ((CSL_DDR2_MDDR_SDCR2_ROWSIZE_9ROW << CSL_DDR2_MDDR_SDCR2_ROWSIZE_SHIFT) |
                      (CSL_DDR2_MDDR_SDCR2_PASR_4BNK << CSL_DDR2_MDDR_SDCR2_PASR_SHIFT));
    
    /* Control refresh rate                                                   */
    ddrRegs->SDRCR  = (0x492 << CSL_DDR2_MDDR_SDRCR_RR_SHIFT);


    /* Set the DDR2 to syncreset, self refresh and enable clkstop             */
    ddrRegs->SDRCR |= ((CSL_DDR2_MDDR_SDRCR_LPMODEN_NO_LPMODE << CSL_DDR2_MDDR_SDRCR_LPMODEN_SHIFT) |
                       (CSL_DDR2_MDDR_SDRCR_MCLKSTOPEN_MCLKSTOP_EN << CSL_DDR2_MDDR_SDRCR_MCLKSTOPEN_SHIFT));
                       
    /*SyncReset the Clock                                                     */
    psc1Regs->MDCTL[CSL_PSC_DDR2_MDDR] =((psc1Regs->MDCTL[CSL_PSC_DDR2_MDDR] & 
                                      ~(CSL_PSC_MDCTL_NEXT_MASK))    | 
                                      CSL_PSC_MDCTL_NEXT_SYNCRST);
    /* Set the transition*/
    psc1Regs->PTCMD = (CSL_PSC_PTCMD_GO0_SET << CSL_PSC_PTCMD_GO0_SHIFT);
    /* Wait for the transition to complete*/    
    while ((((psc1Regs->PTSTAT & CSL_PSC_PTSTAT_GOSTAT0_MASK) >> CSL_PSC_PTSTAT_GOSTAT0_SHIFT)
              == CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION))
    {
        ;
    }
    /* Check for the completion*/
    while((psc1Regs->MDSTAT[CSL_PSC_DDR2_MDDR] & CSL_PSC_MDSTAT_STATE_MASK) != CSL_PSC_MDSTAT_STATE_SYNCRST)
    {
        ;
    }
    
    /*Enable the Clock                                                        */
    psc1Regs->MDCTL[CSL_PSC_DDR2_MDDR] =((psc1Regs->MDCTL[CSL_PSC_DDR2_MDDR] & 
                                     ~(CSL_PSC_MDCTL_NEXT_MASK))    | 
                                     CSL_PSC_MDCTL_NEXT_ENABLE);
    /* Set the transition*/
    psc1Regs->PTCMD = (CSL_PSC_PTCMD_GO0_SET << CSL_PSC_PTCMD_GO0_SHIFT);
    
    /* Wait for the transition to complete*/    
    while ((((psc1Regs->PTSTAT & CSL_PSC_PTSTAT_GOSTAT0_MASK) >> CSL_PSC_PTSTAT_GOSTAT0_SHIFT)
              == CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION))
    {
        ;
    }
    /* Check for the completion*/
    while((psc1Regs->MDSTAT[CSL_PSC_DDR2_MDDR] & CSL_PSC_MDSTAT_STATE_MASK) != CSL_PSC_MDSTAT_STATE_ENABLE)
    {
        ;
    }

    /* Disable self refresh                                                    */
    ddrRegs->SDRCR &= ~((CSL_DDR2_MDDR_SDRCR_LPMODEN_NO_LPMODE << CSL_DDR2_MDDR_SDRCR_LPMODEN_SHIFT) |
                       (CSL_DDR2_MDDR_SDRCR_MCLKSTOPEN_MCLKSTOP_EN << CSL_DDR2_MDDR_SDRCR_MCLKSTOPEN_SHIFT)); 
    return 0;
}

int ddrTest( void )
{
        Int16 i, errors = 0;

    ddr_base = 0xc0004000; 
    ddr_size = 0x00010000;

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

    ddr_base = 0xc0004000; 
    ddr_size = 0x03FFC000;

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

void delay(Uint32 _delayCount)
{
    volatile Uint32 delayCount = _delayCount;
    
    while(0 != delayCount)
    {
        delayCount--;
    }
    
    return;
}
