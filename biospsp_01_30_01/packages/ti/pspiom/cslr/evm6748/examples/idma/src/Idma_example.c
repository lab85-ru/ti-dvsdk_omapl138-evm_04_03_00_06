/*
 * Idma_example.c
 *
 * This file contains the test / demo code to demonstrate basic IDMA operations 
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
 *  @brief  A simple example to demonstrate CSL IDMA register layer usage.
 *
 *  @file  idma_example.c
 *
 *  <b> Example description </b>
 *  @verbatim
    The IDMA example sets up a simple internal memory to internal memory
    transfer using the IDMA.  The IDMA will transfer 20 32-bit words from the
    source buffer to the destination buffer.  The contents of the buffers are
    compared and a pass\fail status will be returned from main.
    @endverbatim
 *
 *
 *  @n <b> Procedure to run the example </b>
 *  @verbatim
    1. Configure the CCS setup to work with the emulator being used.
    2. Please refer CCS manual for setup configuration and loading
       proper GEL file.
    3. Launch CCS.
    4. Open project Idma_example.pjt.
    5. Build the project and load the .out file for the project.
    @endverbatim
 *
 *  ============================================================================
 **/

#include <stdio.h>
#include <ti/pspiom/cslr/cslr_idma.h>
#include <ti/pspiom/cslr/soc_C6748.h>

/* Macros                                                                     */
#define DATA_COUNT_BYTES   80
#define DATA_CNT    20

/* Global Variables                                                           */

/* IDMA Register Overlay                                                      */
CSL_IdmaRegsOvly idmaRegs = (CSL_IdmaRegsOvly)CSL_IDMA_0_REGS;

/* Source buffer                                                              */
const unsigned int src[DATA_CNT] =
 {
    0xDEADBEEF, 0xFADEBABE, 0x5AA51C3A, 0xD4536BA3,
    0x5E69BA23, 0x4884A01F, 0x9265ACDA, 0xFFFF0123,
    0xBEADDABE, 0x234A76B2, 0x9675ABCD, 0xABCDEF12,
    0xEEEECDEA, 0x01234567, 0x00000000, 0xFEEDFADE,
    0x0A1B2C3D, 0x4E5F6B7C, 0x5AA5ECCE, 0xFABEFACE
 };

/* Destination buffer                                                         */
unsigned int dst[DATA_CNT];

/* Prototypes Declaration                                                     */
static void init_idma(void);
static int test_idma(void);


int main(void)
{
  /*Initialize IDMA and perform transfer                                      */
  init_idma();

  /*Verify destination buffer matches source buffer                           */
  return(test_idma());
}

void init_idma(void)
{
  unsigned int restore_value;

  /* Save interrupt state                                                     */
  restore_value = _disable_interrupts();

  /* Set source                                                               */
  idmaRegs->IDMA1_SOURCE = (Uint32)src;
  idmaRegs->IDMA1_DEST = (Uint32)dst;         // Set destination

  /* Set the priority to low and turn on cpu interrupt enable                  *
   * Set the transfer count                                                   */
  idmaRegs->IDMA1_COUNT = CSL_FMKT(IDMA_IDMA1_COUNT_PRI,PRI7)
                        | CSL_FMKT(IDMA_IDMA1_COUNT_INT, INT)
                        | CSL_FMK(IDMA_IDMA1_COUNT_COUNT,DATA_COUNT_BYTES);

  /* Wait while the transfer is active                                        */
  while(CSL_FEXT(idmaRegs->IDMA1_STAT,IDMA_IDMA1_STAT_ACTV) == 1);

  /* Restore interrupt state                                                  */
  _restore_interrupts(restore_value);
}

int test_idma(void)
{
  int index;

  printf("\nTesting IDMA transfer\n");

  /* Compare source and destination buffers                                   */
  for(index=0;index<DATA_CNT;index++)
  {
      if (src[index] != dst[index])
      {
         printf("\nIDMA data copy test:  FAILED\n");
         return(1);
      }
  }

  printf("\nIDMA data copy test:  PASSED\n");

  return(0);
}

