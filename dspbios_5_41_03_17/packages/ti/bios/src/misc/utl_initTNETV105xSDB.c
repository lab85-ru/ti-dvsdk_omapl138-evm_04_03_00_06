/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 * ======== utl_initTNETV105xSDB ========
 * This file contains the function that sets up the PLL
 * for the TNETV105xSDB. The function needs to be used as an initialization 
 * function which will be called before main(). Use following tconf options. 
 * bios.GBL.CALLUSERINTFXN = true;
 * bios.GBL.USERINITFXN = prog.extern("UTL_initTNETV105xSDB");
 *
 */

#pragma CODE_SECTION(UTL_initTNETV105xSDB, ".sysinit");
#pragma CODE_SECTION(pllSetClock, ".sysinit");
#pragma CODE_SECTION(pcntlFree, ".sysinit");
#pragma CODE_SECTION(pcntlReg32Read, ".sysinit");
#pragma CODE_SECTION(pcntlReg32Write, ".sysinit");
#pragma CODE_SECTION(pcntlConfig, ".sysinit");
#pragma CODE_SECTION(perRegRead, ".sysinit");
#pragma CODE_SECTION(perRegWrite, ".sysinit");

#include <std.h>

#define REQUIRED_DIVIDER        1
#define REQUIRED_MULTIPLIER     5

/* Register Definitions of PLL peripheral */
#define SCLKPCR         0x05080101ul

/* Data bitfields for Clock Mode Register (SCLKPCR) */
#define PLSTATUS                        0         /* Read only */
#define PLSTATUS_SZ                     1
#define PLLNDIV                         1
#define PLLNDIV_SZ                      1
#define PLON_OFF                        2
#define PLON_OFF_SZ                     1
#define PLCOUNT                         3
#define PLCOUNT_SZ                      8
#define PLDIV                           11
#define PLDIV_SZ                        1
#define PLMUL                           12
#define PLMUL_SZ                        4 

/* SCLKPCR Register Configuration Values */
#define VDSP_PLL_RESET_DIVMODE      (Uint32) 0x00000000
#define VDSP_PLL_RESET_PLLMODE      (Uint32) 0x00000002

#define PLMODE_PLL                  ((Uint32) 0x1 << PLLNDIV)
#define PLL_ON                      (0x1 << PLON_OFF)

#define VDSP_PLCOUNT 0x7f /* Assuming Lockup time = 44 micro sec 
                             for 50 Mhz Clock                   */

#define VDSP_PLL_DIV_1 1
#define VDSP_PLL_DIV_2 2
#define VDSP_PLL_DIV_4 4

/* Register Definition  of PCNTL module               */

/* pcntl Addr is 32-bit width, and it consists of following fields
 * 1. Bit 0-4 :  Contains the offset of peripheral register [0 to 31] 
 * with respect to the Access window page
 * 2. Bit 7-17:  Access window Page number [0 to 2047]
 * 3. Bit 18:   '0' Selects the peripheral, '1' Selects PDMA
 * 4. Bit 24-27: Shared peripheral module address indentifing the 
 * peripheral
 *  5. Bit 28-31: 0001                                          
 */

#define PCNTL_BASE_ADDR          0x8000
#define PCNTL_PROXY_OFFSET       0x0080
#define PCNTL_PID_VALUE          0x00030401
#define PCNTL_PID_ADDR              (PCNTL_BASE_ADDR) 
#define PCNTL_PID_OFFSET            0
#define PCNTL_ACCESS_CNTL_ADDR      (PCNTL_BASE_ADDR+2)
#define PCNTL_ACCESS_CNTL_OFFSET    1
#define PCNTL_ADDR_REG_ADDR         (PCNTL_BASE_ADDR+4)
#define PCNTL_ADDR_REG_OFFSET       2
#define PCNTL_WR_REQ_ADDR           (PCNTL_BASE_ADDR+6)
#define PCNTL_WR_REQ_OFFSET         3
#define PCNTL_RD_REQ_ADDR           (PCNTL_BASE_ADDR+8)   
#define PCNTL_RD_REQ_OFFSET         4

/*
 *  There are 32 proxy registers in PCNTL module. To access any proxy register 
 *  pass on the appropriate offset. For example to access first proxy register 
 *  pass 0 as the offset
 */
#define MAX_NO_OF_PROXY_REGS        ((Uint16)32)
#define PROXY(offset)               (PCNTL_BASE_ADDR + PCNTL_PROXY_OFFSET + offset*2)

#define PCNTL_FREE                  1
#define PCNTL_BUSY                  0
#define INITIATE_TRANSFER       0x80000000 // Initiate Transfer of Proxy register

/* MACRO FUNCTIONS */
#define CONTENTS_OF(addr) \
        (*((IoPort)(addr)))

#define IO_REG_READ(addr) \
        (CONTENTS_OF(addr))

#define IO_REG_WRITE(addr,val) \
        (CONTENTS_OF(addr) = (val))

/* Structure definitions                                          */
typedef struct 
{   
    Uint32   pid;                        /* Peripheral ID  Register */
    Uint32   accessCntrl;                /* Access Control Register */
    Uint32   addressReg;                 /* Address Register        */
    Uint32   wrReq;                      /* Write Request Register  */
    Uint32   rdReq;                      /* Read Request Register   */
} pcntlRegs_t ;  /* PCNTL Registers Structure */

typedef ioport volatile unsigned int* IoPort;

Uint32 pcntlReg32Read (Uint16 addr) ;
void pcntlReg32Write (Uint16 addr, Uint32 value) ;
Uint16 pcntlFree (void) ;
Uint32 perRegRead (Uint32 addr) ;
void perRegWrite (Uint32 addr, Uint32 reg_val) ;
void pcntlConfig (pcntlRegs_t *pcntl) ;
Bool pllSetClock(Uint16 clk_divider, Uint16 clk_multiplier);


/*  ========  UTL_initTNETV105xSDB ======== */
Void UTL_initTNETV105xSDB(Void)
{
    pllSetClock(REQUIRED_DIVIDER, REQUIRED_MULTIPLIER);
}

/*  ========  pllSetClock ======== */
Bool pllSetClock(Uint16 clk_divider, Uint16 clk_multiplier)
{
    Uint16 plmult = 0;
    Bool pldiv;
    Uint32 reg_value;
    volatile Uint32 mode_check;

    /* Get the PLL in DIV mode */
    perRegWrite(SCLKPCR,(Uint32)VDSP_PLL_RESET_DIVMODE);                

    mode_check = 0x1 << PLSTATUS;
    while (mode_check & (0x1 << PLSTATUS)) { /* wait for DIV mode to be active */
        mode_check = perRegRead(SCLKPCR);
    }

    switch(clk_divider)
    {
       case VDSP_PLL_DIV_1: /* Ratio = clk_multiplier / 1 */
              pldiv  = 0;
              if(clk_multiplier == 1)
                plmult = 0xf;
              else
                plmult = clk_multiplier - 1; 
              break;

       case VDSP_PLL_DIV_2: /* Ratio = clk_multiplier / 2 */
              if(clk_multiplier % 2)
              { /* Odd Multiplier  */
                pldiv  = 1;
                plmult = clk_multiplier - 1;
              }
              else
              { /* Even multiplier */
                pldiv  = 0;
                plmult = (clk_multiplier/2) - 1; 
              }
              break;

       case VDSP_PLL_DIV_4: /* Ratio = clk_multiplier / 4 */
              if(clk_multiplier % 2)
              { /* Odd multiplier */
                pldiv  = 1;
                plmult = clk_multiplier;
              }
              else if((clk_multiplier/2)%2) 
              { /* Even Multiplier not divisible by four */
                pldiv  = 1;
                plmult = (clk_multiplier/2) - 1;
              }
              else
              { /* Even Multiplier divisible by four */
                 pldiv  = 0;
                 plmult = (clk_multiplier/4) - 1;
              }
              break;
    }

    reg_value =    ((Uint32)VDSP_PLCOUNT << PLCOUNT)
                   | ((Uint32)plmult << PLMUL)
                   | PLMODE_PLL
                   | PLL_ON;

    if(pldiv)
        reg_value |= (Uint32)0x1 << PLDIV;              /* Divide by one      */
    else
        reg_value &= ~((Uint32)0x1 << PLDIV);   /* Divide by two/four */

    perRegWrite(SCLKPCR,0x4);
    perRegWrite(SCLKPCR,reg_value);     

    mode_check = 0;
    while (!(mode_check & (0x1 << PLSTATUS))) { 
        /* wait for PLL mode to be active */
        mode_check = perRegRead(SCLKPCR);
    }
        
    return(TRUE);
}


/*  ======== pcntlFree ======== 
 * FUNCTION PURPOSE: To Check for the PCNTL status (FREE or BUSY)   
 * DESCRIPTION: This rountine checks whether PCNTL is busy or not by reading
 * PCNTL access control register 
 *
 * unsigned int pcntlFree(void)
 *
 * Input: None
 * Returns: "1" if the PCNTL is free
 *         "0" if the PCNTL is busy
 */
Uint16 pcntlFree(void)
{
    Uint16 ready, msw;

    msw = IO_REG_READ(PCNTL_ACCESS_CNTL_ADDR);

    /*Check the bit 15 (=MSB) */
    ready = ((msw & 0x8000)>>15);
    
    if (ready == 0) {
        return(PCNTL_FREE);
    } /*End of IF Block*/
    else{
        return(PCNTL_BUSY);
    } /*End of IF-ELSE Block*/
    
} /* End of pcntlFree() */


/*  ======== pcntlReg32Read ========
 * FUNCTION PURPOSE: To read the 32-bit PCNTL registers
 * DESCRIPTION: This rountine reads PCNTL register.
 *
 * Uint32 pcntlReg32Read (Uint16 addr)
 *
 * Input:  PCNTL Register Address
 * Returns: Content of register
 *
 */
Uint32 pcntlReg32Read (Uint16 addr) 
{
    Uint16 msw, lsw ;
    Uint32 retVal;
    
    msw = IO_REG_READ(addr) ;
    lsw = IO_REG_READ(addr+1) ;
    
    retVal = (((Uint32)msw<<16) | ((Uint32)lsw));
    return (retVal);
    
}/* End of pcntlReg32Read(addr) */


/*  ======== pcntlReg32Write ========
 * FUNCTION PURPOSE: To write into the 32-bit PCNTL registers
 * DESCRIPTION: This rountine writes into a PCNTL register.
 * 
 * void pcntlReg32Write (  Uint16 addr,    -- PCNTL Register Address
 *                         Uint32 value   -- Value to be written to the 
 *                        )                           register 
 *
 * Returns: None
 */

void pcntlReg32Write (Uint16 addr, Uint32 value) 
{
    Uint16 msw , lsw;
    msw=(Uint16)((value&0xffff0000)>>16);
    lsw=(Uint16)value&0x0000ffff;

    IO_REG_WRITE((addr+1), lsw);
    IO_REG_WRITE(addr, msw);
        
}/* End of pcntlReg32Write(addr, value) */


/*  ======== pcntlConfig ========
 * FUNCTION PURPOSE: To configure  pcntl module for perfoming specified action
 * DESCRIPTION: This rountine waits for PCNTL module to become free. Once the 
 *              module becomes free, PCNTL registers are configured to complete 
 *              a peripheral read or a write. The function polls for the 
 *              completion of PCNTL Access before returning.
 *              The function can be used when there are few registers that need 
 *              to be accessed.
 *
 *  void pcntlConfig (pcntlRegs_t *pcntl  -- Pointer to pcntlRegs_t object
 *                   ) 
 *  Returns: none
 */

void pcntlConfig(pcntlRegs_t *pcntl) 
{
   /* Check for PCNTL free, write to proxy if the PCNTL is free */
   while (!pcntlFree());       
   
   /* Program PCNTL Address Register          */
   pcntlReg32Write (PCNTL_ADDR_REG_ADDR, pcntl->addressReg);

   /* Program PCNTL Read Request Reg          */
   pcntlReg32Write (PCNTL_RD_REQ_ADDR, pcntl->rdReq);

   /* Program PCNTL Write Request Reg          */
   pcntlReg32Write (PCNTL_WR_REQ_ADDR, pcntl->wrReq);       

   IO_REG_WRITE(PCNTL_ACCESS_CNTL_ADDR,((pcntl->accessCntrl & 0xffff0000)>>16));

   pcntl->pid=pcntlReg32Read(PCNTL_PID_ADDR);
   
   /* Wait for PCNTL transfer to complete      */
   while (!pcntlFree());       

} /* End of pcntlConfig() */


/*  ======== perRegRead ========
 * FUNCTION PURPOSE: To read a peripheral register through PCNTL 
 * DESCRIPTION: This rountine instantiates a PCNTL structure and initializes 
 * various members and calls pcntlConfig() 
 *
 *
 *  Uint32 perRegRead(Uint32 addr) 
 *
 *  Input : 32-bit input of Uint32 data  type
 *             Uint32 data type is 32-bit width, and it consists of 
 *             following fields
 *         1. Bit 0-4 : Contains the offset of peripharal register [0 to 31] 
 *                      with respect to the Access window page 
 *         2. Bit 7-17: Access window Page number [0 to 2047]
 *         3. Bit 18: '0' Selects the peripheral, '1' Selects PDMA
 *         4. Bit 24-27: Shared peripheral module address indentifing the 
 *                       peripheral
 *         5. Bit 28-31: 0001
 *
 * Returns: Value of peripheral register
 */

Uint32 perRegRead (Uint32 addr)
{
    Uint16 pxy_addr, reg_offset ;
    pcntlRegs_t pcntl ;

    reg_offset = (Uint16) (addr & 0x1f) ;

    /*
     * Initialise the PCNTL configuration structure elements, to configure PCNTL
     */

    pcntl.rdReq = (Uint32)0x1 << reg_offset ;
    pcntl.wrReq = (Uint32)0x0 ;
    pcntl.addressReg = addr & 0xFFFFFFE0UL ;   
    pcntl.accessCntrl = INITIATE_TRANSFER ;

   /*
    * write to PCNTL Registers for data transfer between peripheral and
    * Proxy Registers
    */
    pcntlConfig (&pcntl) ;

    /* Read from PCNTL proxy register*/
    pxy_addr = PCNTL_BASE_ADDR + PCNTL_PROXY_OFFSET + (reg_offset*2) ;

    return (pcntlReg32Read (pxy_addr)) ; /*Return 1. */

} /* End of perRegRead() */

/*  ======== perRegWrite ========
 * FUNCTION PURPOSE: To write into a peripheral registers through PCNTL 
 * DESCRIPTION: This rountine instantiates a PCNTL structure and initializes 
 * various members and calls pcntlConfig() 
 *
 *
 *  void perRegWrite(Uint32   addr, Uint32 reg_val)
 *
 *   Input : 32-bit input of Uint32 data  type
 *            Uint32 data type is 32-bit width, and it consists of following 
 *            fields
 *         1. Bit 0-4 : Contains the offset of peripharal register [0 to 31] 
 *                      with respect to the Access window page 
 *         2. Bit 7-17:  Access window Page number [0 to 2047]
 *         3. Bit 18: '0' Selects the peripheral, '1' Selects PDMA
 *         4. Bit 24-27: Shared peripheral module address indentifing the 
 *                       peripheral
 *         5. Bit 28-31: 0001
 *
 *  Output: None
 */
void perRegWrite ( Uint32 addr, Uint32 reg_val)
{
    Uint16 pxy_addr, reg_offset;
    pcntlRegs_t pcntl;

    reg_offset=(Uint16) (addr & 0x1f);

    /* Check for PCNTL free, write to proxy if the PCNTL is free */

    while (!pcntlFree());

    pxy_addr=PCNTL_BASE_ADDR+PCNTL_PROXY_OFFSET+(reg_offset*2);
    pcntlReg32Write(pxy_addr, reg_val) ;

    /* Initialise PCNTL configuration structure elements, to configure PCNTL */

    pcntl.rdReq = (Uint32)0x0 ;
    pcntl.wrReq = (Uint32)0x1 << reg_offset ;
    pcntl.addressReg = addr & 0xFFFFFFE0UL ;
    pcntl.accessCntrl = INITIATE_TRANSFER ;

   /*
    * write to PCNTL Registers for data transfer between peripheral and
    * Proxy Registers 
    */
    pcntlConfig(&pcntl);

} /* End of perRegWrite() */
/* Nothing past this point */







