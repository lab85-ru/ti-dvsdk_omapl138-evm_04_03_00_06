/*
 * C6Run_cio.c
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
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"
#if (0)
  #include "rpc_server.h"
  #include "rpc_memory.h"
#endif

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#if (1)
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif


/*  ----------------------------------- IPC Specific Headers          */
#include "cio_ipc.h"
#include "cio_ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_cio.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/

static int32_t  LOCAL_convToHostOpenFlags(int32_t flags);
static void     LOCAL_cioPerform(struct CIO_Msg *cioMsg);


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

pthread_t C6RUN_CIO_thread;

// Maps DSP file ID numbers to GPP file ID numbers
int32_t fnomap[CIO_NFILE+1];


/************************************************************
* Global Function Definitions                               *
************************************************************/

void *C6RUN_CIO_server ( void *ptr )
{
  uint16_t msgId;  
  
  struct CIO_Msg *cioMsg;
  
  // Set default return value
  C6RUN_retVal = C6RUN_FAIL;
  
  CIO_IPC_allocMsg(&cioMsg);

  // Main loop for receiving and processing C I/O 
  while (1)
  {
    // Read the message from the DSP (blocks forever)
    msgId = CIO_IPC_recvMsg(&cioMsg);
    
    VERBOSE_PRINT0("A CIO message was received.\n");

    if (CIO_NULL == msgId)
    {
      printf("CIO_IPC_recvMsg() returned CIO_NULL.\n"); 
      break;
    }
    else if (CIO_TRANSFER == msgId)
    {
      LOCAL_cioPerform(cioMsg);
      CIO_IPC_sendMsg(cioMsg, CIO_TRANSFER_RET);
    }
    else if (CIO_TERMINATE == msgId)
    {
      CIO_IPC_sendMsg(cioMsg, CIO_TERMINATE_RET);
      C6RUN_retVal = C6RUN_SUCCESS;
      break;
    }
    else
    {
      printf("Unexpected msgId: %d\n", msgId);
      printf("Terminating CIO server.\n");
      break;
    }
  }

  pthread_exit((void *)&C6RUN_retVal);
}


int C6RUN_CIO_init( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    VERBOSE_PRINT0("Entered C6RUN_CIO_init()\n");

    if (C6RUN_SUCCESS != CIO_IPC_init())
    {
      printf("CIO_IPC_init() failed.\n");
      return C6RUN_FAIL;
    }
    
    LOCAL_isModuleInitialized = true;
    
    VERBOSE_PRINT0("Leaving C6RUN_CIO_init()\n");
  }
  return C6RUN_SUCCESS;  
}

// C6Run CIO cleanup routine
void C6RUN_CIO_exit( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_CIO_exit()\n");
  
  CIO_IPC_exit();
  LOCAL_isModuleInitialized = false;
  
  VERBOSE_PRINT0("Leaving C6RUN_CIO_exit()\n");
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/

//-----------------------------------------------------------------------------
// LOCAL_convToHostOpenFlags
//-----------------------------------------------------------------------------
static int32_t LOCAL_convToHostOpenFlags(int32_t flags)
{
  int32_t new_flags = 0;

  if ((flags & (TRG_O_RDONLY | TRG_O_WRONLY | TRG_O_RDWR)) == TRG_O_RDONLY)
  {
    new_flags |= O_RDONLY;
  }
  if (flags & TRG_O_WRONLY)  new_flags |= O_WRONLY;
  if (flags & TRG_O_RDWR)    new_flags |= O_RDWR;
  if (flags & TRG_O_APPEND)  new_flags |= O_APPEND;
  if (flags & TRG_O_CREAT)   new_flags |= O_CREAT;
  if (flags & TRG_O_TRUNC)   new_flags |= O_TRUNC;

  // If ever ported to a Windows system, have to worry about binary files

  return new_flags;
}

// Function to perform C I/O command requests sent from the DSP
static void LOCAL_cioPerform(struct CIO_Msg *cioMsg)
{
  long  result;
  int32_t   fileid;
  unsigned long count;
  int32_t flags;
  char  *cp;
  uint8_t command, parmbuf[8], buffer[CIO_MSG_TOTALSIZE];

  VERBOSE_PRINT0("Entered C6RUN_CIO_perform()\n");

  CIO_IPC_readMsg(cioMsg, &command, parmbuf, buffer);

  /*------------------------------------------------------------------------*/
  /*   HANDLE EACH COMMAND REQUEST                                          */
  /*------------------------------------------------------------------------*/
  switch (command)
  {
    case _DTOPEN:
    {
      /*----------------------------------------------------------------*/
      /*  _DTOPEN - OPEN THE FILE AND MAP THE FILE DESCRIPTOR           */
      /*             IF IT SUCCEEDED                                    */
      /*----------------------------------------------------------------*/
      fileid = UNLOADINT(parmbuf,0);
      if ((fileid < 0) || (fileid > CIO_NFILE)) {
          printf("Invalid file id in CIO message (%d)\n", fileid);
          return;
      }

      VERBOSE_PRINT2("-OPEN: <%s> fileid: %d\n",(char *)buffer,fileid);
      flags = UNLOADINT(parmbuf, 2);
      flags = LOCAL_convToHostOpenFlags(flags);
      result = open((const char *) buffer, flags, S_IREAD | S_IWRITE);
      if (result >= 0) fnomap[fileid] = result;

      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg( cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTCLOSE:
    {
      /*----------------------------------------------------------------*/
      /*  _DTCLOSE - CLOSE THE FILE IF NOT STDIN, STDOUT OR STDERR      */
      /*----------------------------------------------------------------*/
      fileid = UNLOADINT(parmbuf,0);
      if ((fileid < 0) || (fileid > CIO_NFILE)) {
          printf("Invalid file id in CIO message (%d)\n",fileid);
          return;
      }
      VERBOSE_PRINT1("-CLOSE: fileid: %d\n",fileid);

      result= (fnomap[fileid] <= 2) ? 0 : close(fnomap[fileid]);

      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg( cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTWRITE:
    {
      /*----------------------------------------------------------------*/
      /*  _DTWRITE - WRITE FROM THE FILE                                */
      /*----------------------------------------------------------------*/
      fileid = UNLOADINT(parmbuf,0);
      if ((fileid < 0) || (fileid > CIO_NFILE)) {
          printf("Invalid file id in CIO message (%d)\n", fileid);
          return;
      }
      count = UNLOADINT(parmbuf,2);
      if (count > (CIO_MSG_TOTALSIZE+1)) {
          printf("Corrupt CIO message: buffer length (%d) > buffer size (%d)\n",(int)count,(int)CIO_MSG_TOTALSIZE+1);
          return;
      }
      VERBOSE_PRINT1("-WRITE: fileid: %d\n", fileid);

      if (fileid == 1)
      {
        // This check is inserted since an issue was seen where piping stdout
        // to a file would result in debug printf's in this application getting
        // out of order with respect to the data written via the write API.  
        // Since this would be very confusing for any end user, all writes to 
        // stdout (fileid==1) are converted to printf calls.  Performance may
        // suffer but functionality is maintained.
        buffer[count] = 0x00;
        result = printf((char *)buffer);
      }
      else
      {
        result = write(fnomap[fileid], buffer, count);
      }

      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg( cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTREAD:
    {
      /*----------------------------------------------------------------*/
      /*  _DTREAD - READ FROM THE FILE                                  */
      /*----------------------------------------------------------------*/
      fileid = UNLOADINT(parmbuf,0);
      if ((fileid < 0) || (fileid > CIO_NFILE)) {
          printf("Invalid file id in CIO message (%d)\n",fileid);
          return;
      }
      count = UNLOADINT(parmbuf,2);
      if (count > (CIO_MSG_TOTALSIZE+1)) {
          printf("Corrupt CIO message: buffer length (%d) > buffer size (%d)\n", (int)count, (int)CIO_MSG_TOTALSIZE+1);
          return;
      }
      VERBOSE_PRINT1("-READ: fileid: %d\n",fileid);

      result = read(fnomap[fileid], buffer, count);

      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf,(result <= 0) ? NULL : buffer,
                   (result < 0) ? 0 : result);

      break;
    }

    case _DTLSEEK:
    {
      /*----------------------------------------------------------------*/
      /*  _DTLSEEK - PERFORM THE LSEEK ON THE FILE DESCRIPTOR           */
      /*----------------------------------------------------------------*/
      VERBOSE_PRINT0(" -LSEEK\n");

      result = lseek(fnomap[UNLOADINT(parmbuf,0)],
                     UNLOADLONG(parmbuf,2), UNLOADINT(parmbuf,6));

      LOADLONG(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTUNLINK:
    {
      /*----------------------------------------------------------------*/
      /*  _DTUNLINK - PERFORM UNLINK ON THE PATHNAME                    */
      /*----------------------------------------------------------------*/
      VERBOSE_PRINT1(" -UNLINK <%s>\n", (char *)buffer);

      result = unlink((char *) buffer);
      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTRENAME:
    {
      /*----------------------------------------------------------------*/
      /*  _DTRENAME - RENAME OLD FILENAME TO NEW FILENAME               */
      /*----------------------------------------------------------------*/
      VERBOSE_PRINT1(" -RENAME <%s>\n", (char *)buffer);

      result = rename((char *) buffer,
                    ((char *) buffer + strlen((char *)buffer) + 1));
      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
    }

    case _DTGETENV:
    {
      /*----------------------------------------------------------------*/
      /*  _DTGETENV - PERFORM GETENV ON THE PASSED STRING               */
      /*----------------------------------------------------------------*/
      VERBOSE_PRINT1(" -GETENV <%s>\n", (char *)buffer);
      result = ((cp = getenv((char *) buffer)) != NULL);
      LOADINT(parmbuf,result,0);
      if (cp)
      {
       strcpy((char *) buffer,cp);
      }
      else
      {
       buffer[0] = '\0';
      }
      CIO_IPC_writeMsg(cioMsg, parmbuf,buffer,strlen((char *) buffer) + 1);
      break;
    }

    case _DTGETTIME:
    {
      /*----------------------------------------------------------------*/
      /*  _DTGETTIME - PERFORM GETTIME                                  */
      /*----------------------------------------------------------------*/
      VERBOSE_PRINT0(" -GETTIME\n");
      result = time(NULL);

      LOADLONG(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
    }      

    case _DTGETCLK:
    {
      /*----------------------------------------------------------------*/
      /*  _DTGETCLK - PERFORM GETCLOCK                                  */
      /*----------------------------------------------------------------*/
      struct timeval now;
      
      VERBOSE_PRINT0(" -GETCLOCK\n" );

      gettimeofday (&now, NULL);
      result = (int32_t) (now.tv_usec + (now.tv_sec * 1000000));
      //result = clock();

      LOADLONG(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
    }
    // FIXME - all the dsp-rpc-posix stuff still needs to be properly
    // integrated
#if (0)
    /*----------------------------------------------------------------*/
    /*  RPC server temporarily resides here                           */
    /*----------------------------------------------------------------*/
    case RPC_MSG_REQUEST:
    {
        uint8_t *rpc_res;
        
        VERBOSE_PRINT0("Received RPC request\n");
        
        memcpy(rpc_rmessage_buffer, buffer, RPC_BUFSZ);
        rpc_unmarshal();
        rpc_locate_stub();
        rpc_execute_stub();
        rpc_process_result();
        
        rpc_res = (uint8_t *)rpc_get_result_buffer();
        LOADLONG(parmbuf,RPC_MSG_RESPONSE,0);
        
        CIO_IPC_writeMsg(parmbuf,rpc_res,RPC_MAX_PARAM_LEN);
        break;
    }
    
    case RPC_MSG_MALLOC:
    {
        uint8_t *rpc_res;
        unsigned int size = UNLOADLONG(buffer, 0);

        VERBOSE_PRINT1("Received rpc_malloc request, requested size is %d \n", size);

        // Allocate contiguous shared memory using C6Run's CMEM-based allocator
        void *result_virtual = C6RUN_MEM_malloc(size);
        void *result_physical = NULL;
        
        // Get physical address of allocated memory (the DSP works with physical addresses)
        if(result_virtual)
        {
            result_physical = C6RUN_MEM_virt2Phys(result_virtual);
        }
        
        // Add map entry
        #if (0)
        rpc_add_mapping(result_virtual, result_physical, size);
        #endif
        
        // Load the result into the buffer and pass it via MSGQ
        rpc_res = (uint8_t *)rpc_get_result_buffer();
        LOADLONG(rpc_res, (uint32_t)result_physical, 0);
        LOADLONG(parmbuf,RPC_MSG_RESPONSE,0);
        CIO_IPC_writeMsg(parmbuf,rpc_res,RPC_MAX_PARAM_LEN);
        break;
    }
    
    case RPC_MSG_FREE:
    {
        uint8_t *rpc_res;
        unsigned int phys_addr = UNLOADLONG(buffer, 0);

        VERBOSE_PRINT1("Received rpc_free request, buffer pointer is %x \n", phys_addr);
        
        // Translate to virtual address
        void *virt_addr = C6RUN_MEM_virt2Phys((void *)phys_addr);
        
        // Remove mapping entry
        #if (0)
        rpc_remove_mapping(phys_addr);
        #endif
        
        // Free the memory block
        C6RUN_MEM_free(virt_addr);
        
        // Load NULL result into the buffer and pass it via MSGQ
        // Otherwise the DSP will freeze waiting for the answer
        rpc_res = (uint8_t *)rpc_get_result_buffer();
        LOADLONG(rpc_res, 0, 0);
        LOADLONG(parmbuf,RPC_MSG_RESPONSE,0);
        CIO_IPC_writeMsg(parmbuf,rpc_res,RPC_MAX_PARAM_LEN);
        break;
    }
    
    case RPC_MSG_TRANSLATE:
    {
        uint8_t *rpc_res;
        unsigned int src_addr = UNLOADLONG(buffer, 0), type = UNLOADLONG(buffer, sizeof(unsigned int));

        VERBOSE_PRINT2("Received rpc_translate_address request, source address %x and translation type %d \n", src_addr, type);

        // Use RPC translator to perform the translation as desired
        unsigned int result = rpc_translate_address(src_addr, type);
        // Load the result into the buffer and pass it via MSGQ
        rpc_res = (uint8_t *)rpc_get_result_buffer();
        LOADLONG(rpc_res, result, 0);
        LOADLONG(parmbuf,RPC_MSG_RESPONSE,0);
        CIO_IPC_writeMsg(parmbuf,rpc_res,RPC_MAX_PARAM_LEN);
        break;
    }    
#endif
    default:
      /*----------------------------------------------------------------*/
      /* Unrecognized command.  Send a NULL result back to DSP in an    */
      /* attempt to prevent it from hanging.                            */
      /*----------------------------------------------------------------*/
      printf("Invalid CIO command (%d)\n", command);
      result = 0;
      LOADINT(parmbuf,result,0);
      CIO_IPC_writeMsg(cioMsg, parmbuf, NULL, 0);
      break;
  }

  VERBOSE_PRINT0("Leaving C6RUN_CIO_perform\n");
}

/***********************************************************
* End file                                                 *
***********************************************************/
