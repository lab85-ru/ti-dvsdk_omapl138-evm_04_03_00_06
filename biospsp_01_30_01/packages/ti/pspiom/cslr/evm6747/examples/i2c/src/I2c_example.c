/*
 * I2c_example.c
 *
 * This file contains the test / demo code to demonstrate basic I2C operations 
 * using the Regsiter CSL macros.
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
 *  @brief  A simple example to demonstrate CSL I2C register layer usage.
 *
 *  @file  I2c_example.c
 *
 *  <b> Example description </b>
 *  @verbatim
    The I2C example sets up the i2c interface in loopback mode and will transmit
    and receive 8 characters and verify that they are correct.  A pass\fail
    status will be returned from main.
    @endverbatim
 *
 *
 *  @n <b> Procedure to run the example </b>
 *  @verbatim
    1. Configure the CCS setup to work with the emulator being used.
    2. Please refer CCS manual for setup configuration and loading
       proper GEL file.
    3. Launch CCS.
    4. Open project I2c_example.pjt.
    5. Build the project and load the .out file for the project.
    @endverbatim
 *
 *  ============================================================================
 **/

#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/soc_C6747.h>
#include <ti/pspiom/cslr/cslr_i2c.h>
#include <stdio.h>
#define NUM_CHARS       8

static void init_i2c(void);
static int test_i2c(void);

CSL_I2cRegsOvly i2cRegs = (CSL_I2cRegsOvly)CSL_I2C_0_REGS;


int main (void)
{
  //setup i2c registers and start i2c running
  init_i2c();

  //test loopback, returns 0 for pass and 1 for fail
  return(test_i2c());

}

void init_i2c(void)
{
  //i2c mode register
  i2cRegs->ICMDR = CSL_FMKT(I2C_ICMDR_MST,MASTER_MODE) //master mode
                            | CSL_FMKT(I2C_ICMDR_TRX,TX_MODE) //transmitter mode
                            | CSL_FMKT(I2C_ICMDR_RM,ENABLE) //repeat mode
                            | CSL_FMKT(I2C_ICMDR_DLB,ENABLE) //digital loopback
                            | CSL_FMKT(I2C_ICMDR_STT,SET); //enable start

  //set i2c clock low time divide down value, 0-FFFFh
  i2cRegs->ICCLKL = CSL_FMK(I2C_ICCLKL_ICCL,0x10);

  //set i2c clock high time divide down value, 0-FFFFh
  i2cRegs->ICCLKH = CSL_FMK(I2C_ICCLKH_ICCH,0x10);

  //set i2c prescalar divide down value, 0-FFh
  i2cRegs->ICPSC = CSL_FMK(I2C_ICPSC_IPSC,0x2);

  //bring i2c module out of reset
  CSL_FINST(i2cRegs->ICMDR,I2C_ICMDR_IRS,ENABLE);

}

int test_i2c(void)
{
 int i;
 char trans_val[NUM_CHARS]={'l','o','o','p','b','a','c','k'};
 char rec_val[NUM_CHARS];

 printf("\nTesting I2C loopback.\n\n");

 for(i=0; i<(sizeof(trans_val)/sizeof(trans_val[0])); i++)
 {
   //poll transmitter ready
   while(CSL_FEXT(i2cRegs->ICSTR,I2C_ICSTR_ICXRDY)!= 1);

   //write to transmit register
   i2cRegs->ICDXR = trans_val[i];

   //poll receiver is ready
   while(CSL_FEXT(i2cRegs->ICSTR,I2C_ICSTR_ICRRDY)!= 1);

   //read receive register
   rec_val[i] = i2cRegs->ICDRR;
 }

 for(i=0; i<(sizeof(trans_val)/sizeof(trans_val[0])); i++)
 {
   if(trans_val[i]==rec_val[i])
     printf("transmit: %c  receive: %c\n",trans_val[i],rec_val[i]);
   else
     {
       printf(" error\n");
       printf("\nI2C Loopback Test:  FAILED");
       return(1);
     }
 }
 printf("\nI2C Loopback Test:  PASSED\n");
 printf("\nLoopback test complete.\n");
 return(0);
}
