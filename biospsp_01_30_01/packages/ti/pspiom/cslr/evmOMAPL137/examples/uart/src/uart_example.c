/*
 * uart_example.c
 *
 * This file contains the test / demo code to demonstrate basic UART 
 * operations using the Regsiter CSL macros.
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
 *   @brief A simple example to demonstrate CSL  UART register layer usage.
 *
 *   @file  Uart_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
     This example sets up the UART in 8-bit loopback mode.  Characters are
     transmitted and received using software polling. The test will transmit
     8 characters and receive 8 characters and verify the results are correct.
     A pass\fail result will be returned from main.
     @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
     1. Configure the CCS setup to work with the emulator being used
     2. Please refer CCS manual for setup configuration and loading
        proper GEL file
     3. Launch CCS window
     4. Open project Uart_example.pjt
     5. Build the project and load the .out file of the project.
     @endverbatim
 *
 * =============================================================================
 **/

#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_uart.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL137.h>
#include <stdio.h>

#define NUM_CHARS    8

static void device_init(void);
static void init_uart(void);
static int test_uart(void);

CSL_UartRegsOvly uartRegs = (CSL_UartRegsOvly)CSL_UART_0_REGS;

int main (void)
{
  //enable uart in the power and sleep controller
  device_init();

  //setup uart registers and start uart running
  init_uart();

  //test loopback, returns 0 for pass and 1 for fail
  return(test_uart());

}

void init_uart(void)
{

  //line control register
  uartRegs->LCR = CSL_FMKT(UART_LCR_WLS,8BITS); //word length 8bits

  //modem control register
  uartRegs->MCR = CSL_FMKT(UART_MCR_RTS,ENABLE)  //RTS control
                          | CSL_FMKT(UART_MCR_LOOP,ENABLE) //loopback mode
                          | CSL_FMKT(UART_MCR_AFE,ENABLE); //auto flow enable

  //divisor latch
  CSL_FINS(uartRegs->DLL,UART_DLL_DLL,0x1);

  //enable transmitter and receiver
  CSL_FINST(uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_UTRST,ENABLE);
  CSL_FINST(uartRegs->PWREMU_MGMT,UART_PWREMU_MGMT_URRST,ENABLE);
}

int test_uart(void)
{
 int i;
 char trans_val[NUM_CHARS]={'l','o','o','p','b','a','c','k'};
 char rec_val[NUM_CHARS];

 printf("\nTesting UART loopback.\n\n");

 for(i=0;i<(sizeof(trans_val)/sizeof(trans_val[0]));i++)
 {
   //poll transmitter ready
   while(CSL_FEXT(uartRegs->LSR,UART_LSR_THRE)!= 1);

   //write to transmit register
   CSL_FINS(uartRegs->THR,UART_THR_DATA,trans_val[i]);

   //poll receiver is ready
   while(CSL_FEXT(uartRegs->LSR,UART_LSR_DR)!= 1);

   //read receive register
   rec_val[i] = CSL_FEXT(uartRegs->RBR,UART_RBR_DATA);
 }

 for(i=0;i<(sizeof(trans_val)/sizeof(trans_val[0]));i++)
 {
   if(trans_val[i]==rec_val[i])
     printf("transmit: %c  receive: %c\n",trans_val[i],rec_val[i]);
   else
     {
      printf(" error\n");
      printf("\nUART loopback test:  FAILED\n");
      return(1);
     }
 }

 printf("\nUART loopback test:  PASSED\n");
 printf("\nLoopback test complete.\n");
 return(0);
}



void device_init(void)
{

  CSL_PscRegsOvly psc0Regs = (CSL_PscRegsOvly)CSL_PSC_0_REGS;

  // deassert UART local PSC reset and set NEXT state to ENABLE
  psc0Regs->MDCTL[CSL_PSC_UART0] = CSL_FMKT( PSC_MDCTL_NEXT, ENABLE )
                               | CSL_FMKT( PSC_MDCTL_LRST, DEASSERT );
  // move UART PSC to Next state
  psc0Regs->PTCMD = CSL_FMKT(  PSC_PTCMD_GO0, SET );

  // wait for transition
  while ( CSL_FEXT( psc0Regs->MDSTAT[CSL_PSC_UART0], PSC_MDSTAT_STATE )
          != CSL_PSC_MDSTAT_STATE_ENABLE );

}

