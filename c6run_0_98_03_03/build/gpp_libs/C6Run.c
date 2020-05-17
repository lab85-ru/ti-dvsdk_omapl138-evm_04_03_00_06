/*
 * C6Run.c
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"
#include "C6Run_ipc.h"
#include "C6Run_cio.h"
#include "C6Run_rpc.h"
#include "C6Run_control.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "control_ipc.h"
#include "control_ipc_common.h"



/************************************************************
* Explicit External Declarations                            *
************************************************************/

// External references to dsp image data and size
extern const unsigned char  _dsp_program_data_ [];
extern const unsigned int   _dsp_program_data_size_;

extern void *C6RUN_CIO_server ( void *ptr );


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

// C6RUN_Type
typedef enum _C6RUN_TYPE_
{
  C6RUN_TYPE_NONE     = 0, 
  C6RUN_TYPE_APP      = 1, 
  C6RUN_TYPE_LIB      = 2
}
C6RUN_Type;


/************************************************************
* Local Function Declarations                               *
************************************************************/

// Internal initialization
static int LOCAL_C6RUN_init ( int argc, char **argv, C6RUN_Type progType );

// Signal handling routines
static void LOCAL_installSignalHandlers(int numSignals, int *signalList);
static void LOCAL_sigHandler (int sigNum);


/************************************************************
* Local Variable Definitions                                *
************************************************************/

// List of OS signals to handle
static int signalList[8] =
{
  SIGHUP,
  SIGINT,
  SIGILL,
  SIGABRT,
  SIGFPE,
  SIGSEGV,
  SIGALRM,
  SIGTERM
} ;

static bool LOCAL_isAppInitialized = false;
static bool LOCAL_isLibInitialized = false;
static bool LOCAL_isModuleInitialized = false;

static void *C6RUN_CONTROL_msg;

/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile C6RUN_Type C6RUN_type      = C6RUN_TYPE_NONE;
volatile int C6RUN_retVal           = 0;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t C6RUN_init( void )
{
  if (!LOCAL_isLibInitialized)
  {
    return LOCAL_C6RUN_init(0, NULL, C6RUN_TYPE_LIB);
  }
  else
  {
    return C6RUN_SUCCESS;
  }
}

void C6RUN_exit ( void )
{
  // Exit now if init has not yet been done
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_exit().\n");
  
  // For C6RunLib, send a message to tell the DSP to stop.
  // Otherwise we just wait for the DSP to tell us it is finished.
  if (C6RUN_TYPE_LIB == C6RUN_type)
  {
    C6RUN_CONTROL_sendMsg(C6RUN_CONTROL_msg, CONTROL_DSP_STOP);
  }

  // Pend on receipt of the DSP_FINISHED message
  if (CONTROL_DSP_FINISHED != C6RUN_CONTROL_recvMsg(&C6RUN_CONTROL_msg))
  { 
    // Report if the wrong message ID was received  
    printf("CONTROL_DSP_FINISHED not received.\n");
  }
  else
  {
    VERBOSE_PRINT0("CONTROL_DSP_FINISHED was received.\n");
  }

  // Get the return value if this is C6RunApp
  if (C6RUN_TYPE_APP == C6RUN_type)
  {
    volatile int *p;
  
    // Get return value from the DSP processor
    p = (volatile int *) C6RUN_CONTROL_getMsgBuffer(C6RUN_CONTROL_msg);
 
    // Message buffer only contains an int32_t return value from main executing on DSP
    C6RUN_retVal = *p;

    VERBOSE_PRINT1("DSP return value = %d.\n",C6RUN_retVal);
  }
  // Shut down the RPC communication if this is C6RunLib
  else if (C6RUN_TYPE_LIB == C6RUN_type)
  {
    C6RUN_RPC_exit();
  }
  
  // Delete the control message
  C6RUN_CONTROL_freeMsgBuff(C6RUN_CONTROL_msg);
  
  // C6RUN_CIO_thread to shutdown
  pthread_join(C6RUN_CIO_thread, NULL);

  VERBOSE_PRINT0("Completed shutdown of CIO thread.\n");
  
  // Gracefully shut everything else down (IPC, DSP, etc.)
  C6RUN_IPC_delete(DSP0);

  // Reset the initialized variable
  LOCAL_isModuleInitialized = false;

  VERBOSE_PRINT0("Leaving C6RUN_exit().\n");
}

int C6RUN_appInit(int argc, char **argv)
{
  if (!LOCAL_isAppInitialized)
  {
    return LOCAL_C6RUN_init(argc, argv, C6RUN_TYPE_APP);
  }
  else
  {
    return C6RUN_SUCCESS;
  }
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/

static int LOCAL_C6RUN_init ( int argc, char **argv, C6RUN_Type progType )
{
  int32_t status = 0;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_init()\n");
  
  if ((C6RUN_TYPE_LIB != progType) && (C6RUN_TYPE_APP != progType))
  {
    return C6RUN_FAIL;
  }

  // Save the program type (so we can later figure out how to wait for termination)
  C6RUN_type = progType;
    
  // Get DSP running, set up MSGQ's, etc.
  status = C6RUN_IPC_create((uint8_t *)_dsp_program_data_, _dsp_program_data_size_, DSP0);
  if (status < 0)
  {
    printf("C6RUN_IPC_create() failed!\n");
    exit(1);
  }
  VERBOSE_PRINT0("C6RUN_IPC_create() passed!\n");

  //FIXME - Replace this with C6RUN_CIO_init() (it should init its own receiver thread)
  // Start the C6RUN CIO server task
  if (pthread_create(&C6RUN_CIO_thread, NULL, C6RUN_CIO_server, NULL) != 0)
  {
    printf("Aborting...pthread_create() for CIO server failed!\n");
    exit(1);
  }
  VERBOSE_PRINT0("pthread_create() passed!\n");

  // Allocate control message
  C6RUN_CONTROL_allocMsgBuff(&C6RUN_CONTROL_msg);
  
  if (C6RUN_TYPE_LIB == C6RUN_type)
  {
    // Send signal to DSP to start
    C6RUN_CONTROL_sendMsg(C6RUN_CONTROL_msg, CONTROL_DSP_STARTLIB);
    
    // Init the RPC layer
    if ( C6RUN_SUCCESS != C6RUN_RPC_init() )
    {
      printf("C6RUN_RPC_init() failed\n");
      exit(1);
    }
      
    // Pend on receipt of the DSP_READY message for C6RunLib projects
    if ( CONTROL_DSP_READY != C6RUN_CONTROL_recvMsg(&C6RUN_CONTROL_msg) )
    {
      printf("CONTROL_DSP_READY not recv'd\n");
      exit(1);
    }
    VERBOSE_PRINT0("CONTROL_DSP_READY recv'd!\n");
  }
  else if (C6RUN_TYPE_APP == C6RUN_type)
  {
    // Fill in the main() function argument buffer
    /* The control message payload looks like the following: 
     *              argc,
     *              argv[0], -------+
     *              argv[1], -------|--+
     *              ...,            |  |
     *              argv[n], -------|--|--+
     *              argv 0 string,<-+  |  |
     *              argv 1 string,<----+  |
     *              ...                   |
     *              argv n string,<-------+   
     */
 
    int i;
    uint32_t  *argsPtr = C6RUN_CONTROL_getMsgBuffer(C6RUN_CONTROL_msg);
    uintptr_t *argvPtr = NULL;
    uint32_t  argLength;
    uint32_t  totalSize, availableSize;

    totalSize = (uint32_t) ( sizeof(int) + argc * sizeof(uintptr_t) );
    availableSize = C6RUN_CONTROL_getMsgBufferSize();
    
    *argsPtr++ = argc;
    argvPtr = (uintptr_t *) argsPtr;

    // First argument data starts at end of list of argv pointers
    argvPtr[0] = (uintptr_t) (argvPtr + argc);  
    argLength = strlen (argv [0]) + 1;
    argLength = (argLength + sizeof(uintptr_t) - 1) & (~(sizeof(uintptr_t) - 1));
    
    totalSize += argLength;
    
    if (totalSize > availableSize)
    {
      printf("Command-line arguments do not fit (desired size = 0x%08X > available size = 0x%08X)!\n",totalSize, availableSize);
      exit(1);
    }
      
    // If we haven't run out of room, copy the first argument string
    memcpy((void *) argvPtr[0], (void *) argv [0], argLength);
    
    // Loop through checking and copying the remainder arguments
    for (i = 1 ; i < argc; i++)
    {
      argvPtr[i] = argvPtr[i-1] + argLength;
      argLength = strlen (argv [i]) + 1;
      argLength = (argLength + sizeof(uintptr_t) - 1) & (~(sizeof(uintptr_t) - 1));
      
      totalSize += argLength;
    
      if (totalSize > availableSize)
      {
        printf("Command-line arguments do not fit (desired size = 0x%08X > available size = 0x%08X)!\n",totalSize, availableSize);
        exit(1);
      }      
      
      // If we haven't run out of room, copy the argument string data
      memcpy((void *) argvPtr[i], (void *) argv [i], argLength);
    }
    
    // Install the signal handlers we need to correctly kill this program
    // We don't install signal handlers for the lib case (let the user app figure this out)
    LOCAL_installSignalHandlers(sizeof(signalList)/sizeof(signalList[0]), signalList);    
  
    // Send the startup message (with the argument info in the buffer)
    C6RUN_CONTROL_sendMsg(C6RUN_CONTROL_msg, CONTROL_DSP_STARTAPP);
  }
  
  // Register exit routine for calling upon program termination
  // We do this for both app and lib use cases
  atexit(C6RUN_exit);
  
  LOCAL_isModuleInitialized = true;

  VERBOSE_PRINT0("Leaving C6RUN_init().\n");
  
  return C6RUN_SUCCESS;
}

static void LOCAL_installSignalHandlers(int numSignals, int *signalList)
{
  int i;
  struct sigaction sigInstaller ;
  sigset_t block_mask ;

  sigemptyset( &block_mask );
  sigfillset ( &block_mask );
  sigInstaller.sa_flags   = (SA_RESTART);
  sigInstaller.sa_mask    = block_mask;
  sigInstaller.sa_handler = &LOCAL_sigHandler;

  // Actually register the handler for the different signals
  for (i = 0 ; i < numSignals ; i++)
  {
    sigaction (signalList[i], &sigInstaller, NULL) ;
  }
}

static void LOCAL_sigHandler (int sigNum)
{
  // Output signal handling message
  VERBOSE_PRINT1("Signal %d received! Aborting...",sigNum);
  
  /* It is necessary to call the cleanup function
   * from signal handler context to ensure that the
   * cleanup is not interrupted by another signal.
   * Just exit (0) is not enough  as the at exit handler
   * can be pre-empted by a signal.
   */
  C6RUN_exit ();

  /*  Force hard stop on the process  */
  exit (0);

  return;
}


/***********************************************************
* End file                                                 *
***********************************************************/
