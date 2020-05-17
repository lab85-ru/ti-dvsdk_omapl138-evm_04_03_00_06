/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/*
 *  ======== audio_main.c ========
 */


/* OS Specific Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* DSP/BIOS Link */
#include <gpptypes.h>
#include <dsplink.h>
#include <errbase.h>

/* DSP/BIOS LINK API */
#include <proc.h>
#include <msgq.h>
#include <pool.h>

/* Common header file shared between GPP and DSP */
#include "../common/common.h"

/* Number of pools */
#define NUMMSGPOOLS    3

/* 
 * Zero-copy message queue attributes based on physical link used by 
 * Message Queue Transport (MQT)
 */
STATIC ZCPYMQT_Attrs  mqtAttrs ;

/* Application data buffer */
STATIC Uint16 * AppDataBuf;

/* Message sizes managed by the pool */
STATIC Uint32 AppBufSizes [NUMMSGPOOLS] =
{
    BUFSIZE,             /* Contiguous application buffer */
    APPMSGSIZE,          /* Application messages needed for message passing */
    ZCPYMQT_CTRLMSG_SIZE /* Control messages needed by message transport */
}; 

/* Number of message in each pool managed by the pool */
STATIC Uint32 AppNumBuffers [NUMMSGPOOLS] =
{
    2,     /* Number of BUFSIZE application buffers */
    4,     /* Number of APPMSGSIZE application messages */
    4      /* Number of ZCPYMQT_CTRLMSG_SIZE conto messages */
};

/* Attributes for the pool used based on phtsical link used by MQT */
STATIC SMAPOOL_Attrs AppPoolAttrs =
{
    NUMMSGPOOLS,    /* Number of pools */
    AppBufSizes,    /* Size of buffers in each pool */
    AppNumBuffers,  /* The number of buffers in each pool */
    TRUE            /* Exact match */
} ;

/* Name of the Message Queue (MSGQ) on the GPP  */
STATIC Char8 gppMsgqName [DSP_MAX_STRLEN] = "outputGPP" ;

/* GPP's Message Queue (MSGQ) Object */
STATIC MSGQ_Queue gppMsgq = (Uint32) MSGQ_INVALIDMSGQ ;

/* Name of the Message Queue (MSGQ) on the DSP  */
STATIC Char8 dspMsgqName [DSP_MAX_STRLEN] = "inputDSP" ;

/* DSP's Message Queue (MSGQ) Object */
STATIC MSGQ_Queue dspMsgq = (Uint32) MSGQ_INVALIDMSGQ ;

/* terminate flag for signal handler */
Bool terminateFlag = FALSE;

/* Signal Handler function */
void signalHandler(int sig);

/** ============================================================================
 *  @func   main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
int main (int argc, char ** argv)
{
    Char8 * dspExecutable    = NULL ;
    Char8 * audioFile        = NULL ;
    Char8 * strProcessorId   = NULL ;
    Uint8   processorId      = 0    ;
    DSP_STATUS       status  = DSP_SOK  ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    FILE    *inFile = NULL;
    Int16    bufferCount = 0;
    Int32    readBytes;
    MSGQ_LocateAttrs syncLocateAttrs ;
    MyMsg * msg ;
    Void * dspDataBuf;
    Uint8 retVal = 0;

    if ((argc!=3)) {
        printf ("Usage : %s <absolute path of DSP executable> "
           " <PCM audio file>\n", argv [0]) ;
	retVal = -1;
	goto terminate;
    }

    dspExecutable    = argv [1] ;
    audioFile        = argv [2] ;

    /* Set DSP processor id */
    strProcessorId   = "0" ;
    processorId       = 0   ;
    
    /* Open the input file */
    inFile = fopen(audioFile, "rb");

    if (inFile == NULL) {
        printf("Failed to open input file\n");
        retVal = -1;
        goto terminate;

    }
       
    if (processorId < MAX_PROCESSORS) {

        /* Setup application */
        printf ("========== Application started ==========\n") ;
        printf ("Calling PROC_setup() \n");
        status = PROC_setup (NULL);

        if (DSP_FAILED (status)) {
            printf("PROC_setup() failed. Status = [0x%x]\n",
                (Uint16)status);
	    retVal = -1;
            goto terminate;
        }               
            
        /* Attach to the Dsp */
        printf ("Calling PROC_attach ()\n");
        status = PROC_attach (processorId, NULL) ;

        if (DSP_FAILED (status)) {
            printf("PROC_attach () failed. Status = [0x%x]\n",
                (Uint16)status) ;
            retVal = -1;
            goto destroy;
        }               

        /* Open up the a specific pool from which to allocate buffers */
        printf ("Calling POOL_open ()\n");
        status = POOL_open (
                    POOL_makePoolId(processorId, APP_POOL_ID),
                        &AppPoolAttrs);

        if (DSP_FAILED (status)) {
            printf("POOL_open () failed. Status = [0x%x]\n",
                (Uint16)status);
            retVal = -1;
            goto detach;
        }

        /* Allocate a buffer from the pool*/
        printf ("Calling POOL_alloc ()\n");
        status = POOL_alloc (POOL_makePoolId(processorId, APP_POOL_ID),
                     (Void **) &AppDataBuf, BUFSIZE) ;

        if (DSP_FAILED (status)) {
            printf ("POOL_alloc () failed. Status = [0x%x]\n",
                (Uint16)status) ;
            retVal = -1;
            goto poolClose;
        } 

        /* Translate buffer address to pass to DSP */
        printf ("Calling POOL translate address\n");
        status = POOL_translateAddr (
                    POOL_makePoolId(processorId, APP_POOL_ID),
                       &dspDataBuf, AddrType_Dsp, (Void *)AppDataBuf,
                       AddrType_Usr);

        if (DSP_FAILED (status)) {
            printf ("POOL_translateAddr () dspDataBuf failed."
                " Status = [0x%x]\n", (Uint16)status) ;
            retVal = -1;
            goto poolFree;
        } 
            
        /* Open the GPP's message queue */
        printf ("Calling MSGQ_open () for GPP MSGQ \n");
        status = MSGQ_open (gppMsgqName, &gppMsgq, NULL) ;

        if (DSP_FAILED (status)) {
            printf ("MSGQ_open () failed. Status = [0x%x]\n",
                (Uint16)status) ;
            retVal = -1;
            goto poolClose;
        }

        /* Load the executable on the DSP */
        printf ("Calling PROC_load () \n");
        status = PROC_load (processorId, dspExecutable, 0, NULL);
        
        if (DSP_FAILED (status)) {
            printf ("PROC_load () failed. Status = [0x%x]\n", 
	        (Uint16)status) ;
            retVal = -1;
            goto msgqClose;
        }

        /* Start execution on DSP */
        printf ("Calling PROC_start () \n");
        status = PROC_start (processorId) ;
             
        if (DSP_FAILED (status)) {
            printf ("PROC_start () failed. Status = [0x%x]\n",
                (Uint16)status) ;
            retVal = -1;
            goto msgqClose;
        }
            
        /* Open the remote transport */
        printf ("Calling POOL_makePoolId ()\n");
        mqtAttrs.poolId = POOL_makePoolId(processorId, APP_POOL_ID)  ;
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;

        if (DSP_FAILED (status)) {
            printf ("MSGQ_transportOpen () failed. Status = [0x%x]\n",
                (Uint16)status) ;
            retVal = -1;
            goto stop;
        }
            
        /*  Locate the DSP's message queue */
        syncLocateAttrs.timeout = WAIT_FOREVER;
        status = DSP_ENOTFOUND ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            printf ("Calling MSGQ locate for DSP message\n") ;
            status = MSGQ_locate (dspMsgqName, &dspMsgq,
                    &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                usleep (100000) ;
                printf ("Sleeping Waiting to locate DSP Message\n") ;
            }
            else if (DSP_FAILED (status)) {
                printf ("MSGQ_locate () failed. Status = [0x%x]\n",
                    (Uint16)status) ;
                retVal = -1;
                goto transClose;
            }
        }
        printf ("DSP message located\n") ;
            
        /* Register signal handler function for Ctrl C */
        (void) signal(SIGINT, signalHandler);

        while (1) {
            /* Read raw PCM datad from input file to buffer data */
            readBytes = fread(AppDataBuf, 1, BUFSIZE, inFile);

            bufferCount++;

            /* Check signal handler terminate flag */
            if (terminateFlag) {
                /* Set readBytes to zero to begin terminate */
                readBytes = 0;
            }

            if (readBytes == 0) {
                if (ferror(inFile)) {
                    printf("Failed to read data from input file\n");
                    goto cleanup;
                }
                printf ("Total messages sent and received %d\n", bufferCount);

		/* Get a message and send message back for termination */
                status = MSGQ_get (gppMsgq, WAIT_FOREVER, (MSGQ_Msg *)&msg) ;
                MSGQ_setMsgId (msg, TERMINATEMSGID) ; 
                status = MSGQ_put (dspMsgq, (MSGQ_Msg)msg) ;
                printf ("Send terminate message to DSP\n") ;

		/* Waiting for termination message back */
                status = MSGQ_get (gppMsgq, WAIT_FOREVER, (MSGQ_Msg *)&msg) ;
                printf ("Begin termination\n") ;
                goto cleanup;
            }

	    /* Perform a cache writeback to ensure data in memory is valid */
            POOL_writeback (POOL_makePoolId(processorId, APP_POOL_ID),
                    AppDataBuf, BUFSIZE) ;

            /* Receive the message */
            status = MSGQ_get (gppMsgq, WAIT_FOREVER, (MSGQ_Msg *)&msg) ;
            if (bufferCount % 100 == 0) {
                printf("Recieved %i messages for GPP to process\n",bufferCount);
            }

            if (DSP_FAILED (status)) {
                printf ("MSGQ_get () failed. Status = [0x%x]\n",
                    (Uint16)status) ;
            }

	    /* Place data buffer pointer into message and set Message id */
            msg->dataBuffer = dspDataBuf;
            MSGQ_setMsgId (msg, GPP_OUTPUTMSGID) ; 

            /*Send the same message received in earlier MSGQ_get () call */
            status = MSGQ_put (dspMsgq, (MSGQ_Msg)msg) ;
            if (bufferCount % 100 == 0) {
                printf ("Sent %i messages for DSP to process\n", bufferCount) ;
            }
            if (DSP_FAILED (status)) {
                MSGQ_free ((MSGQ_Msg)msg) ;
                printf ("MSGQ_put () failed. Status = [0x%x]\n",
                    (Uint16)status) ;
            }
                
        }

cleanup: 
        /* Close the input file */
        if (inFile) {
            fclose(inFile);
        }
            
        /* Release the remote message queue */
        printf ("Calling MSGQ_release ()\n");
        status = MSGQ_release (dspMsgq) ;

        if (DSP_FAILED (status)) {
            printf ("MSGQ_release () failed. Status = [0x%x]\n", 
                (Uint16)status) ;
        }

transClose:
	/* Close the remote transport */
        printf ("Calling MSGQ_transportClose ()\n");
        tmpStatus = MSGQ_transportClose (processorId) ;

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            printf ("MSGQ_transportClose () failed. Status = [0x%x]\n",
                (Uint16)status) ;
        }

stop:
        /* Stop execution on DSP */
        printf ("Calling PROC_stop() \n");
        status = PROC_stop (processorId) ;

        if (DSP_FAILED (status)) {
            printf ("PROC_stop () failed. Status = [0x%x]\n", 
               (Uint16)status) ;
        }

msgqClose:
        /* Close the GPP's message queue */
        tmpStatus = MSGQ_close (gppMsgq) ;

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            printf ("MSGQ_close () failed. Status = [0x%x]\n", 
                (Uint16)status) ;
        }

poolFree:
	/*  Free the buffer allocated by pool */ 
        tmpStatus = POOL_free (POOL_makePoolId(processorId, APP_POOL_ID),
	                (Void *) AppDataBuf, BUFSIZE);

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus;
            printf ("POOL_free () AppDataBuf failed. Status = [0x%x]\n",
                (Uint16)status);
        }
	

poolClose:
        /*  Close the pool */
        tmpStatus = POOL_close (POOL_makePoolId(processorId,
            APP_POOL_ID));

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus;
            printf ("POOL_close () failed. Status = [0x%x]\n", 
		(Uint16)status);
        }

detach:
        /* Detach from the processor */
        printf ("Calling PROC_detach() \n");
        tmpStatus = PROC_detach(processorId);

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            printf ("PROC_detach () failed. Status = [0x%x]\n", 
		(Uint16)status);
        }

destroy:
        /* Destroy the PROC object */
        printf ("Calling PROC_destroy() \n");
        tmpStatus = PROC_destroy();

        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            printf ("PROC_destroy () failed. Status = [0x%x]\n", 
		(Uint16)status);
        }

terminate:
        printf ("========== Application terminated ==========\n") ;

    }

    return (retVal) ;
}

/** ============================================================================
 *  @func   signalHandler
 *
 *  @desc   Signal handler function to catch Ctrl-C
 *
 *  @modif  None
 *  ============================================================================
 */
void signalHandler(int sig)
{
   printf ("Signal handler received: %d ... !!\n", sig) ;
   /* Set terminate flag */
   terminateFlag = TRUE;
}
