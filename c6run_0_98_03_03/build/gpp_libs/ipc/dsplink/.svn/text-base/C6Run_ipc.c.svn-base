/*
 * C6Run_ipc.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
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

/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API             */
#include <proc.h>
#include <procdefs.h>
#include <msgq.h>
#include <pool.h>
#include <loaderdefs.h>

#if defined(LPM_REQUIRED)
  /*  --------------------------------- LPM API                       */
  #include <ti/bios/power/lpm.h>
#endif

/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"
#include "C6Run_rpc.h"
#include "C6Run_cio.h"
#include "C6Run_control.h"
#include "C6Run_sharedmem.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "control_ipc_common.h"
#include "cio_ipc_common.h"
#include "rpc_ipc_common.h"
#include "dsplink/ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

// Extern declaration to the default DSP/BIOS LINK configuration structure.
extern LINKCFG_Object LINKCFG_config;

// Platform specific DSP Object for Link configuration
extern LINKCFG_Dsp myDspObject;

// Platform specific memory table pointer for updating DSP memory map
// in Link configuration
extern LINKCFG_MemEntry* myMemTables[];


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/

#if defined(DSP_HAS_MMU)
  static DSP_STATUS LOCAL_mapToDspMMU(uint8_t processorId, uint32_t dspAddr, int32_t size);
#endif

#if defined(LPM_REQUIRED)
  static DSP_STATUS LOCAL_lpmReset( void );
#endif


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static ZCPYMQT_Attrs mqtAttrs;

/* Message sizes managed by the pool */
static Uint32 PoolBufSizes[NUM_MSG_POOLS] =
{
  DSPLINK_ALIGN (CIO_MSG_TOTALSIZE, DSPLINK_BUF_ALIGN),
  DSPLINK_ALIGN (CONTROL_MSG_TOTALSIZE, DSPLINK_BUF_ALIGN),
  DSPLINK_ALIGN (RPC_MSG_TOTALSIZE, DSPLINK_BUF_ALIGN),
  ZCPYMQT_CTRLMSG_SIZE,
  DSPLINK_ALIGN (sizeof(MSGQ_AsyncLocateMsg), DSPLINK_BUF_ALIGN),
  DSPLINK_ALIGN (sizeof(MSGQ_AsyncErrorMsg), DSPLINK_BUF_ALIGN)
};

/* Number of messages in each pool */
static Uint32 PoolNumBuffers[NUM_MSG_POOLS] =
{
  CIO_MSG_CNT,
  CONTROL_MSG_CNT,
  RPC_MSG_CNT,
  ZCPYMQT_MSG_CNT,
  ASYNCLOCATE_MSG_CNT,
  ASYNCERROR_MSG_CNT
};

/* Definition of attributes for the pool based on physical link used by the transport */
static SMAPOOL_Attrs PoolAttrs =
{
  NUM_MSG_POOLS,
  PoolBufSizes,
  PoolNumBuffers,
  FALSE  /* exact match required on size; was TRUE */
};

static LINKCFG_GppOs myGppOsObject = 
{
  FALSE,       /* HANDLESIGNALS  : Should signals be handled for cleanup */
  0u,          /* NUMSIGNALS     : Number of signals to be handled */
  NULL         /* SIGNUMARRAY    : Pointer to the array of signals to be handled */
};


/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile bool C6RUN_IPC_initialized = FALSE;

#if defined(LPM_REQUIRED)
  /* Global variables required to avoid linker errors due to LPM user-space lib */
  int PWRM_doImcopConfig = 0;
  int PWRM_enableImcopAtPowerOn;
#endif

uint8_t C6RUN_IPC_processorId;

/************************************************************
* Global Function Definitions                               *
************************************************************/

// Loading DSP image from shared memory (uses CMEM heap allocated buffer)
// and initializing the DSPLink message queue interface
int32_t C6RUN_IPC_create(uint8_t* dspImgData, int32_t dspImgSize, uint8_t processorId)
{
  DSP_STATUS status = DSP_SOK;

  COFFLOADER_ImageInfo image; 
  uint32_t srcAddr, dstAddr;

  VERBOSE_PRINT0("Entered C6RUN_IPC_create()\n");

#if defined(LPM_REQUIRED)
  status = LOCAL_lpmReset();
  if ( DSP_FAILED(status) )
  {
    printf("LOCAL_lpmReset () failed. Status = [0x%x]\n", (uint32_t)status);
    return C6RUN_FAIL;
  }
#endif

  // Update DSP object in Link's DSP config
  LINKCFG_config.dspConfigs[processorId]->dspObject = (LINKCFG_Dsp *) &myDspObject;
  
  // Update memory map for the DSP
  LINKCFG_config.dspConfigs[processorId]->memTables = (LINKCFG_MemEntry **) myMemTables;
  
  // Change signal handling to remove DSPLink's control for the ARM OS
  LINKCFG_config.gppObject->gppOsObject = (LINKCFG_GppOs *) &myGppOsObject;
  
  // Save off the processor ID in use
  C6RUN_IPC_processorId = processorId;
  
  // Allocate memory from CMEM heap for DSP load image
  srcAddr = (uint32_t) C6RUN_MEM_memalign(DSPLINK_BUF_ALIGN, DSPLINK_ALIGN (dspImgSize, DSPLINK_BUF_ALIGN));
  if ((void *)srcAddr == NULL)
  {
    printf("Memory allocation for DSP image has failed.\n");
    return C6RUN_FAIL;
  }
  
  // Copy input dsp image data to allocated CMEM buffer
  memcpy((void *)srcAddr, dspImgData, dspImgSize);
  C6RUN_CACHE_wb((void *)srcAddr, dspImgSize);
  
  // Get physical address of CMEM buffer
  dstAddr = (uint32_t) C6RUN_SHAREDMEM_local2Remote((void *)srcAddr);
  
  // Create and initialize the PROC object
  status = PROC_setup( NULL );  
  if ( DSP_FAILED(status) )
  {
    printf("PROC_setup () failed. Status = [0x%x]\n", (uint32_t) status);
    return C6RUN_FAIL;
  }
  VERBOSE_PRINT0("PROC_setup passed!\n");

  // Attach to the DSP with which the transfers have to be done
  if (DSP_SUCCEEDED(status))
  {
    status = PROC_attach(processorId, NULL);
    if ( DSP_FAILED(status) )
    {
      printf("PROC_attach () failed. Status = [0x%x]\n", (uint32_t) status);
      return C6RUN_FAIL;
    }
  }
  VERBOSE_PRINT0("PROC_attach passed!\n");

  // Open the pool for message passing buffers
  if (DSP_SUCCEEDED(status))
  {
    status = POOL_open(POOL_makePoolId(processorId, COMMON_POOL_ID), &PoolAttrs);
    if (DSP_FAILED(status))
    {
      printf("POOL_open () failed. Status = [0x%x]\n", (uint32_t)status);
      return C6RUN_FAIL;
    }
  }
  
  // Load DSP executable image to the attached DSP
  if ( DSP_SUCCEEDED (status) )
  { 
    // Size of files in bytes
    image.size = dspImgSize; 

    // Physical address of memory where COFF file is present
    image.fileAddr = dstAddr;

    // Load the buffer into DSP memory
    status = PROC_load (processorId, (Char8 *) &image, 0, NULL);     
    if (DSP_FAILED(status))
    {
      printf("PROC_load () failed. Status = [0x%x]\n", (uint32_t)status);
      return C6RUN_FAIL;
    }
  }
  VERBOSE_PRINT0("PROC_load() passed!\n");
  
  // Start the DSP running
  if (DSP_SUCCEEDED (status))
  { 
    status = PROC_start (processorId);
    if (DSP_FAILED(status))
    {
      printf("PROC_start () failed. Status = [0x%x]\n", (uint32_t)status);
      return C6RUN_FAIL;
    }
  }
  VERBOSE_PRINT0("PROC_start() passed!\n");

#if defined(DSP_HAS_MMU)
  // Apply MMU settings on OMAP3 platforms only
  LOCAL_mapToDspMMU(processorId, DSP_REGION_BASE_ADDR, DSP_REGION_CMEM_SIZE);
  VERBOSE_PRINT0("LOCAL_mapToDspMMU() passed!\n");
#endif  
  
  // Free the memory allocated from CMEM heap for the DSP load image
  C6RUN_MEM_free((void *)srcAddr);

  // Open the remote transport.
  if (DSP_SUCCEEDED(status))
  {
    mqtAttrs.poolId = POOL_makePoolId(processorId, COMMON_POOL_ID);
    status = MSGQ_transportOpen(processorId, &mqtAttrs);
    if (DSP_FAILED(status))
    {
      printf("MSGQ_transportOpen() failed. Status = [0x%x]\n", (uint32_t)status);
      return C6RUN_FAIL;
    }
  }
  VERBOSE_PRINT0("MSGQ_transportOpen() passed!\n");

  // Now that the IPC interface has been created, init the individual modules
  status |= C6RUN_CONTROL_init();
  status |= C6RUN_CIO_init();  
  
  VERBOSE_PRINT0("Leaving C6RUN_IPC_create\n");
  
  return status;
}

// Teardown of DSPLink interface and shutting off of DSP.  This should
// usually only be called after the DSP code has terminated (exits main()).
void C6RUN_IPC_delete(uint8_t processorId)
{
  DSP_STATUS status = DSP_SOK;
  DSP_STATUS tmpStatus = DSP_SOK;

  VERBOSE_PRINT0("Entered C6RUN_IPC_delete()\n");
  
  // Reset the error handler before deleting the MSGQ that receives
  // the error messages.
  status = MSGQ_setErrorHandler(MSGQ_INVALIDMSGQ, MSGQ_INVALIDMSGQ);
  if (DSP_FAILED(status))
  {
    REPORT_STATUS("MSGQ_setErrorHandler");
  }
  else
  {
    VERBOSE_PRINT0("MSGQ_setErrorHandler() passed.\n");
  }
  
  // Exit the individual IPC modules in reverse order
  C6RUN_CIO_exit();
  C6RUN_CONTROL_exit();

  // Close the remote transport
  tmpStatus = MSGQ_transportClose(processorId);
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("MSGQ_transportClose");
  }
  else
  {
    VERBOSE_PRINT0("MSGQ_transportClose() passed.\n");
  }
  
  // Stop execution on DSP.
  tmpStatus = PROC_stop(processorId);
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("PROC_stop");
  }
  else
  {
    VERBOSE_PRINT0("PROC_stop() passed.\n");
  }

  // Close the pool
  tmpStatus = POOL_close(POOL_makePoolId(processorId, COMMON_POOL_ID));
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("POOL_close");
  }
  else
  {
    VERBOSE_PRINT0("POOL_close() passed.\n");
  }

  // Detach from the processor
  tmpStatus = PROC_detach(processorId);
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("PROC_detach");
  }
  else
  {
    VERBOSE_PRINT0("PROC_detach() passed.\n");
  }

  // Destroy the PROC object.
  tmpStatus = PROC_destroy();
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("PROC_destroy");
  }
  else
  {
    VERBOSE_PRINT0("PROC_destroy() passed.\n");
  }

  VERBOSE_PRINT0("Leaving C6RUN_IPC_delete()\n");
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/

#if defined(LPM_REQUIRED)
/* OMAP3 platforms require a full power-down and power-up of the DSP
   subsystem via the LPM module */
static DSP_STATUS LOCAL_lpmReset( void )
{
  DSP_STATUS status = DSP_EFAIL;
  LPM_Handle hLPM = NULL;

  // Initialize the LPM module.
  LPM_init();

  // Open the LPM object for the given resource.
  if (LPM_open("/dev/lpm0", &hLPM) != LPM_SOK)
  {
    goto LPM_OPEN_FAIL;
  }

  // Turn on power for the resource.
  if (LPM_on(hLPM) != LPM_SOK)
  {
    goto LPM_RESET_FAIL;
  }

  // FIXME - do we need some sort of wait here?
  //sleep(2);  
  
  // Turn off power for the resource.
  if (LPM_off(hLPM) != LPM_SOK)
  {
    goto LPM_RESET_FAIL;
  }  
  
  // FIXME - do we need some sort of wait here?
  //sleep(2);

  // Reset has passed
  status = DSP_SOK;
  
LPM_RESET_FAIL:
  // close the LPM object
  if (LPM_close(hLPM) != LPM_SOK)
  {
    status = DSP_EFAIL;
  }  

LPM_OPEN_FAIL:  
  // Finalize the LPM module.
  LPM_exit();
  
  // Return status
  return status;
}
#endif

#if defined(DSP_HAS_MMU)
/* OMAP3 platforms require the DSP MMU to be aware of memory regions from which
   the ARM may pass data buffers via CMEM */
/* Map area starting at 'dspAddr' (physical address on DSP) with a length of 'size' bytes */
static DSP_STATUS LOCAL_mapToDspMMU(uint8_t processorId, uint32_t dspAddr, int32_t size)
{
  DSP_STATUS status;
  ProcMemMapInfo mapInfo;
 
  mapInfo.dspAddr = (Uint32)dspAddr;
  mapInfo.size = size;
 
  status = PROC_control(processorId, PROC_CTRL_CMD_MMU_ADD_ENTRY, &mapInfo);
 
  return status;
}
#endif


/***********************************************************
* End file                                                 *
***********************************************************/

