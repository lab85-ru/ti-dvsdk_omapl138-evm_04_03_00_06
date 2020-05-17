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

/*  ----------------------------------- SysLink                       */
#include <ti/syslink/Std.h>
#include <ti/syslink/SysLink.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/ProcMgr.h>

/*  ----------------------------------- SysLink Utils/OSAL            */
#include <ti/syslink/utils/Memory.h>

/*  ----------------------------------- SysLink IPC                   */
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/MessageQ.h>


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
#include "syslink/ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern Int IpcDrv_ioctl (UInt32 cmd, Ptr args);


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

#define NUM_REMOTE_PROCS 1

// Enable Tracing of SysLink startup activities
//#define TRACE_PROCMGR_LOAD
//#define TRACE_IPC_CONTROLCMD_LOADCALLBACK
//#define TRACE_PROCMGR_START
//#define TRACE_IPC_CONTROLCMD_STARTCALLBACK


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/

#if defined(DSP_HAS_MMU)
  static int32_t LOCAL_mapToDspMMU(uint8_t processorId, uint32_t dspAddr, int32_t size);
#endif


/************************************************************
* Local Variable Definitions                                *
************************************************************/

// HeapMemMP params that are used for create and delete
static HeapMemMP_Params heapMemParams;

// Handle to underlying HeapMemMP instance
static HeapMemMP_Handle heapHandle;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile bool C6RUN_IPC_initialized = FALSE;

// ProcMgr global arrays 
ProcMgr_Handle ProcMgrApp_handle[NUM_REMOTE_PROCS];
uint32_t ProcMgrApp_fileId[NUM_REMOTE_PROCS];


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Loading DSP image from shared memory (uses CMEM heap allocated buffer)
// and initializing the DSPLink message queue interface
int32_t C6RUN_IPC_create(uint8_t* dspImgData, int32_t dspImgSize, uint8_t processorId)
{
  FILE *pFile;
  
  void *ptr;

  int32_t status = 0;
  ProcMgr_AttachParams         attachParams;
  ProcMgr_StartParams          startParams;
  ProcMgr_State                state;
  
  VERBOSE_PRINT0("Entered C6RUN_IPC_create()\n");

  // Create and initialize the ProcMgr object

  // FIXME: Write DSP image to temp file so it can be loaded by SysLink
  //        This is currently required until SysLink has load support
  //        for memory buffers.
  pFile = fopen("temp.dsp","wb");
  if (!pFile)
  {
    printf("Failed to open or create temp.dsp.\n");
    return C6RUN_FAIL;
  }
  
  // Write dsp image to file
  if (fwrite(dspImgData, sizeof(uint8_t), dspImgSize, pFile) != dspImgSize)
  {
    printf("Failed to write temp.dsp file correctly.\n");
    fclose(pFile);    
    return C6RUN_FAIL;
  }
  fclose(pFile);
  
  // Initialize SysLink
  VERBOSE_PRINT0("Entering SysLink_setup().\n");
  
  SysLink_setup ();
  
  VERBOSE_PRINT0("SysLink_setup() complete.\n");
  
  status = ProcMgr_open (&ProcMgrApp_handle[processorId], processorId);
  if (status < 0)
  {
    printf("ProcMgr_open() failed. Status = [0x%08X]\n", status);
    return C6RUN_FAIL;
  }
  
  // Default params will be used if NULL is passed.
  ProcMgr_getAttachParams (NULL, &attachParams);
  status = ProcMgr_attach (ProcMgrApp_handle[processorId], &attachParams);
  if (status < 0)
  {
    printf("ProcMgr_attach() failed. Status = [0x%08X]\n", status);
    return C6RUN_FAIL;
  }
  
  state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
  VERBOSE_PRINT1 ("After attach: ProcMgr_getState: state [0x%08X]\n", state);

  // Load the temporary DSP image
  {
#ifdef TRACE_PROCMGR_LOAD
    UInt32 oldMask; UInt32 traceClass = 1;
    /* Temporarily enable full trace */ 
    oldMask = GT_setTrace((GT_TraceState_Enable |
       GT_TraceEnter_Enable |
       GT_TraceSetFailure_Enable |
       (traceClass << (32 - GT_TRACECLASS_SHIFT))), 
       GT_TraceType_Kernel);
#endif
    status = ProcMgr_load( ProcMgrApp_handle[processorId],
                         "temp.dsp",
                         0,
                         NULL,
                         NULL,
                         &ProcMgrApp_fileId[processorId] );
    if (status < 0)
    {
      printf("ProcMgr_load() failed. Status = [0x%08X]\n", status);
      return C6RUN_FAIL;
    }
    VERBOSE_PRINT0("ProcMgr_load() passed!\n");

#ifdef TRACE_PROCMGR_LOAD
    /* Restore trace to earlier value */ 
    GT_setTrace(oldMask, GT_TraceType_Kernel);
#endif
  }
  
  // IPC control
  {
#ifdef TRACE_IPC_CONTROLCMD_LOADCALLBACK
    UInt32 oldMask; UInt32 traceClass = 1;
    /* Temporarily enable full trace */ 
    oldMask = GT_setTrace((GT_TraceState_Enable |
       GT_TraceEnter_Enable |
       GT_TraceSetFailure_Enable |
       (traceClass << (32 - GT_TRACECLASS_SHIFT))), 
       GT_TraceType_Kernel);
#endif  
    status = Ipc_control( processorId, Ipc_CONTROLCMD_LOADCALLBACK, NULL );
    if (status < 0)
    {
      printf ("Ipc_control(Ipc_CONTROLCMD_LOADCALLBACK) failed. Status = [0x%08X]\n", status);
      return C6RUN_FAIL;
    }
    
    state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
    VERBOSE_PRINT1 ("After load: ProcMgr_getState: state [0x%08X]\n", state);    
    
#ifdef TRACE_IPC_CONTROLCMD_LOADCALLBACK
    /* Restore trace to earlier value */ 
    GT_setTrace(oldMask, GT_TraceType_Kernel);
#endif
  }

  // Get start params and then start the processor
  {
#ifdef TRACE_PROCMGR_START
    UInt32 oldMask; UInt32 traceClass = 1;
    /* Temporarily enable full trace */ 
    oldMask = GT_setTrace((GT_TraceState_Enable |
       GT_TraceEnter_Enable |
       GT_TraceSetFailure_Enable |
       (traceClass << (32 - GT_TRACECLASS_SHIFT))), 
       GT_TraceType_Kernel);
#endif

    ProcMgr_getStartParams (ProcMgrApp_handle[processorId], &startParams);
    status = ProcMgr_start (ProcMgrApp_handle[processorId], &startParams);
    if (status < 0)
    {
      printf("ProcMgr_start() failed. Status = [0x%08X]\n", status);
      return C6RUN_FAIL;  
    }
    VERBOSE_PRINT0 ("ProcMgr_start() passed!\n");
    
#ifdef TRACE_PROCMGR_START
    /* Restore trace to earlier value */ 
    GT_setTrace(oldMask, GT_TraceType_Kernel);
#endif
  }
  
#if defined(DSP_HAS_MMU)
  // Apply MMU settings on OMAP3 platforms only+
  LOCAL_mapToDspMMU(processorId, DSP_REGION_BASE_ADDR, DSP_REGION_CMEM_SIZE);
  VERBOSE_PRINT0("LOCAL_mapToDspMMU() passed!\n");
#endif    
  
  // IPC control
  {
#ifdef TRACE_IPC_CONTROLCMD_STARTCALLBACK
    UInt32 oldMask; UInt32 traceClass = 1;
    /* Temporarily enable full trace */ 
    oldMask = GT_setTrace((GT_TraceState_Enable |
       GT_TraceEnter_Enable |
       GT_TraceSetFailure_Enable |
       (traceClass << (32 - GT_TRACECLASS_SHIFT))), 
       GT_TraceType_Kernel);
#endif
  
    VERBOSE_PRINT0 ("Before Ipc_control(...,Ipc_CONTROLCMD_STARTCALLBACK,...)!\n");
    status = Ipc_control (processorId, Ipc_CONTROLCMD_STARTCALLBACK, NULL);
    VERBOSE_PRINT0 ("After Ipc_control(...,Ipc_CONTROLCMD_STARTCALLBACK,...)!\n");
    
    if (status < 0)
    {
      printf ("Ipc_control(Ipc_CONTROLCMD_STARTCALLBACK) failed. Status = [0x%08X]\n", status);
      return C6RUN_FAIL;
    }
    
#ifdef TRACE_IPC_CONTROLCMD_STARTCALLBACK
    /* Restore trace to earlier value */ 
    GT_setTrace(oldMask, GT_TraceType_Kernel);
#endif  
  }

  // FIXME - Cleanup debug stuff
  VERBOSE_PRINT0 ("Before ProcMgr_getState()!\n");
  state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
  VERBOSE_PRINT1 ("After start: ProcMgr_getState: state [0x%08X]\n", state);
  
  // Now create and register the heap that will be used as the underlying memory
  // for allocating and freeing messages
  HeapMemMP_Params_init (&heapMemParams);
  heapMemParams.name            = COMMON_HEAP_NAME;  
  heapMemParams.sharedAddr      = NULL;
  heapMemParams.sharedBufSize   = (CIO_MSG_CNT * COMMON_HEAP_ALIGN(CIO_MSG_TOTALSIZE)) + 
             (RPC_MSG_CNT * COMMON_HEAP_ALIGN(RPC_MSG_TOTALSIZE)) + 
             (CONTROL_MSG_CNT * COMMON_HEAP_ALIGN(CONTROL_MSG_TOTALSIZE));

  // Allocate required amount of memory for supporting all required messages
  ptr = Memory_alloc (SharedRegion_getHeap (0), HeapMemMP_sharedMemReq(&heapMemParams), 0, NULL);
  if (ptr == NULL)
  {
    status = MessageQ_E_FAIL;
    printf ("Memory_alloc failed for %d processor. ptr: [0x%08X]\n", processorId, (uint32_t)ptr);
    return C6RUN_FAIL;
  }

  heapMemParams.sharedAddr = ptr;
  heapHandle = HeapMemMP_create (&heapMemParams);
  if (heapHandle == NULL)
  {
    status = MessageQ_E_FAIL;
    printf ("HeapMemMP_create failed for %d processor. Handle: [0x%08X]\n", processorId, (uint32_t)heapHandle);
    return C6RUN_FAIL;
  }

  // Register this heap with MessageQ module
  MessageQ_registerHeap (heapHandle, COMMON_HEAP_ID);
  VERBOSE_PRINT2("Registering heapId %d with MessageQ for procId: %d\n", COMMON_HEAP_ID, processorId);
  
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
  int32_t status = 0, tmpStatus = 0;
  ProcMgr_State  state;  

  VERBOSE_PRINT0("Entered C6RUN_IPC_delete()\n");

  // Exit the individual IPC modules in reverse order
  C6RUN_CIO_exit();
  C6RUN_CONTROL_exit();

  if (ProcMgrApp_handle[processorId] != NULL)
  {
    // Unregister and delete the HeapMemMP instance
    MessageQ_unregisterHeap (COMMON_HEAP_ID);
    status = HeapMemMP_delete (&heapHandle);
    if (status < 0)
    {
      printf ("HeapMemMP_delete() failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("HeapMemMP_delete() passed.\n");
    }
    
    // Free the memory block underlying the HeapMemMP instance
    Memory_free(SharedRegion_getHeap (0), heapMemParams.sharedAddr, HeapMemMP_sharedMemReq(&heapMemParams));
    
    status = Ipc_control( processorId, Ipc_CONTROLCMD_STOPCALLBACK, NULL );
    if (status < 0)
    {
      printf ("Ipc_control(Ipc_CONTROLCMD_STOPCALLBACK) failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("Ipc_control(Ipc_CONTROLCMD_STOPCALLBACK) passed.\n");
    }

    tmpStatus = ProcMgr_stop( ProcMgrApp_handle[processorId] );
    if ((status >= 0) && (tmpStatus < 0))
    {
      status = tmpStatus;
      printf ("ProcMgr_stop() failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("ProcMgr_stop() passed.\n");
    }
    
    state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
    VERBOSE_PRINT1 ("After stop: ProcMgr_getState: state [0x%08X]\n", state);

    tmpStatus = ProcMgr_unload (ProcMgrApp_handle[processorId], ProcMgrApp_fileId[processorId]) ;
    if ((status >= 0) && (tmpStatus < 0))
    {
      status = tmpStatus;
      printf ("ProcMgr_unload() failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("ProcMgr_unload() passed.\n");
    }
    
    state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
    VERBOSE_PRINT1 ("After unload: ProcMgr_getState: state [0x%08X]\n", state);

    tmpStatus = ProcMgr_detach (ProcMgrApp_handle[processorId]);
    if ((status >= 0) && (tmpStatus < 0))
    {
      status = tmpStatus;
      printf ("ProcMgr_detach() failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("ProcMgr_detach() passed.\n");
    }

    state = ProcMgr_getState (ProcMgrApp_handle[processorId]);
    VERBOSE_PRINT1 ("After detach: ProcMgr_getState: state [0x%08X]\n", state);
    
    tmpStatus = ProcMgr_close (&(ProcMgrApp_handle[processorId]));
    if ((status >= 0) && (tmpStatus < 0))
    {
      status = tmpStatus;
      printf ("ProcMgr_close() failed. Status = [0x%08X]\n", status);
    }
    else
    {
      VERBOSE_PRINT0("ProcMgr_close() passed.\n");
    }
  }
  
  SysLink_destroy ();

  VERBOSE_PRINT0("Leaving CONTROL_IPC_delete()\n");
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/

#if defined(DSP_HAS_MMU)
/* OMAP3 platforms require the DSP MMU to be aware of memory regions from which
   the ARM may pass data buffers via CMEM */
/* Map area starting at 'dspAddr' (physical address on DSP) with a length of 'size' bytes */
static int32_t LOCAL_mapToDspMMU(uint8_t processorId, uint32_t dspAddr, int32_t size)
{
  int32_t status = 0;
  ProcMgr_AddrInfo addrInfo;
  
  addrInfo.addr [ProcMgr_AddrType_MasterPhys] = dspAddr;
  addrInfo.addr [ProcMgr_AddrType_SlaveVirt]  = dspAddr;
  addrInfo.size = size;
  addrInfo.isCached = FALSE;
  
  status = ProcMgr_map( ProcMgrApp_handle[processorId],
               ProcMgr_SLAVEVIRT | ProcMgr_MASTERKNLVIRT,
               &addrInfo, 
               ProcMgr_AddrType_MasterPhys);
 
  return status;
}
#endif


/***********************************************************
* End file                                                 *
***********************************************************/

