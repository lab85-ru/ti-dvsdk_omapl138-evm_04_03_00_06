/*
 * Cache_example.c
 *
 * This file contains the test / demo code to demonstrate basic Cache operations 
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
 *  @brief  A simple example to demonstrate CSL  Cache register layer usage.
 *
 *  @file  cache_config_example.c
 *
 *  <b> Example description </b>
 *  @verbatim
    The cache configuration example configures L1P, L1D, and L2 cache using the
    register layer csl.  The configuration is then verified using the BCACHE
    module api's.  A pass\fail status will be returned from main.

    To use BCACHE:
    - BIOS application : simply #include the bcache.h header file. BIOS
    automatically selects the appropriate library based on the chip
    configuration.
    - non-BIOS application : simply #include the bcache.h header file. Link in
    the appropriate bios library and explicitly place the .bios section in your
    linker command file. Unfortunately there is no single bcache.a64P library
    hence the easiest way to determine which library to link in is either (a)
    look at the library these examples link in (b) see which library the
    BIOS examples link in. Note that BCACHE does *not* link in any other BIOS
    modules so you do *not* need a BIOS TCF configuration. Think of BCACHE as
    a standalone module that just happens to be distributed with DSP/BIOS.
    Please reference the BIOS API Reference Guide for a complete list of
    available APIs.

    The code below gives an example of using the BCACHE module read APIs
    in a non-BIOS application
    @endverbatim
 *
 *
 *  @n <b> Procedure to run the example </b>
 *  @verbatim
    1. Configure the CCS setup to work with the emulator being used.
    2. Please refer CCS manual for setup configuration and loading
       proper GEL file.
    3. Launch CCS.
    4. Open project Cache_example.pjt.
    5. Build the project and load the .out file for the project.
    @endverbatim
 *
 *  ============================================================================
 **/

#include <stdio.h>
#include <std.h>
#include <bcache.h>
#include <ti/pspiom/cslr/cslr_cache.h>
#include <ti/pspiom/cslr/soc_C6748.h>

void config_cache(void);
int test_config(void);
int testResult=0;                              // Test status

int main(void)
{
  // Configure the cache using register layer
  config_cache();

  // Verify configuration using the BCACHE module api's
  return(test_config());
}

void config_cache(void)
{
  CSL_CacheRegsOvly cacheRegs = (CSL_CacheRegsOvly)CSL_CACHE_0_REGS;
  volatile unsigned int stall;

  // The below writes to the CFG registers are followed by a dummy read.  Mode
  // switches require that a read is performed immediately after the write.  The
  // read stalls the cpu to ensure the mode change completes.

  // Set L1P size to 32K
  CSL_FINST(cacheRegs->L1PCFG,CACHE_L1PCFG_MODE,32K);
  stall = cacheRegs->L1PCFG;

  // Set L1D size to 32K
  CSL_FINST(cacheRegs->L1DCFG,CACHE_L1DCFG_MODE,32K);
  stall = cacheRegs->L1DCFG;

  // Set L2 size to 64k and normal opperation
  cacheRegs->L2CFG = CSL_FMKT(CACHE_L2CFG_MODE,64K)
                   | CSL_FMKT(CACHE_L2CFG_L2CC,NORMAL);
  stall = cacheRegs->L2CFG;

  // The MAR registers set the cachability of memory spaces external to the
  // megamodule.  Below is an example of turning on cachability for two ranges.
  // Reference spru187 for a complete list the MAR ranges.

  // Set MAR[192] as cacheable
  CSL_FINST(cacheRegs->MAR[192],CACHE_MAR_PC,CACHEABLE);

}

int test_config(void)
{
  BCACHE_Size checkSize;
  BCACHE_Mode checkMode;
  Ptr baseMar192;
  BCACHE_Mar checkMar192;

  baseMar192=(Ptr)0xC0000000;


  // The BCACHE module read API's are used to verify the previous configuration
  // that was performed using the register layer.

  // Read L1P, L1D, and L2 cache size settings
  BCACHE_getSize(&checkSize);

  // Read L2 mode setting
  checkMode = BCACHE_getMode(BCACHE_L2);

  // Read MAR192 setting
  checkMar192 = BCACHE_getMar(baseMar192);

  printf("\nTest cache configuraton\n");

  // Verify cache size settings for L1P, L1D, and L2
  if((checkSize.l1psize==BCACHE_L1_32K)&&(checkSize.l1dsize==BCACHE_L1_32K)&&
     (checkSize.l2size==BCACHE_L2_64K))
     {
       printf("\nL1P size = 32K\n");
       printf("L1D size = 32K\n");
       printf("L2 size = 64K\n");
     }
  else
     {
      printf("\nError setting cache sizes\n");
      testResult=1;
     }

  // Verify mode setting for L2
  if(checkMode==BCACHE_NORMAL)
     printf("\nL2 mode set to NORMAL\n");
  else
     {
      printf("\nError setting mode\n");
      testResult=1;
     }

  // Verify MAR192 settings
  if((checkMar192==BCACHE_MAR_ENABLE))
     printf("\nMAR192 range set to cacheable\n");
  else
     {
      printf("\nError setting MAR range\n");
      testResult=1;
     }

  // Display test status
  if(testResult==0)
     printf("\nCache configuration test:  PASSED\n");
  else
     printf("\nCache configuration test:  FAILED\n");

  return(testResult);
}

