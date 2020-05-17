/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*************************************************************************
* The RTDX Target Library User Interface
*************************************************************************/
#ifndef __RTDX_H
#define __RTDX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>             /* NULL, size_t                         */
#include "RTDX_access.h"        /* RTDX_CODE, RTDX_DATA                 */

/* This controls configuration to be either polling or interrupt driven */
/* It must be defined - values are 0 or 1.                              */
#undef RTDX_POLLING_IMPEMENTATION
#include "rtdxpoll.h"
#ifndef RTDX_POLLING_IMPLEMENTATION
#error RTDX_POLLING_IMPLEMENTATION not defined!
#endif

/* return codes used by read and write functions */
#define RTDX_WRITE_ERROR -3
#define RTDX_CALLBACK_INUSE -2
#define RTDX_READ_ERROR  -1
#define RTDX_FAILURE      0
#define RTDX_OK           1
#define RTDX_WRITE_DONE   2
#define RTDX_READ_DONE    3
#define RTDX_SPACE_AVAIL  4

/*************************************************************************
*  Define Data Types
*
* Data channels are represented by statically declared structures.  A data
* channel may be used either for input or output (logging), but not both.
* The contents of an input or output structure is private to the
* implementation and may change at any time.
*************************************************************************/

#ifndef _RTDX_caction
#define _RTDX_caction
/* Complete action function, channel returned when write completes      */
/* with event RTDX_WRITE_DONE or RTDX_SPACE_AVAIL when space available. */
typedef void RTDX_caction(void *chan, int event);
#endif

/* For Backward Compatability   */
#define RTDX_output_channel RTDX_outputChannel
#define RTDX_input_channel  RTDX_inputChannel

/* --- Data Channels --- */
#define RTDX_CHAN_ENABLED    0x1010

typedef struct RTDX_OUTPUT_CHANNEL {
    /* private */               /* NOTE: enabled MUST BE FIRST FIELD */
    volatile unsigned int  enabled;
    RTDX_caction          *pfWhenDone;  /* called when complete                 */
    struct RTDX_OUTPUT_CHANNEL    *next;  /* list of chans waiting for space */
} RTDX_outputChannel;

#define RTDX_OUTPUT_INITIAL_STATE   {0, 0, 0}

typedef struct RTDX_INPUT_CHANNEL {
    /* private */               /* NOTE: enabled MUST BE FIRST FIELD    */
    volatile unsigned int enabled;
    volatile unsigned int count;/* HOST puts actual bytes received here */
    volatile int busy;          /* 1 if channel busy reading.           */
    RTDX_caction          *pfWhenDone;  /* called when complete                 */
    struct RTDX_INPUT_CHANNEL    *next; /* list of chans waiting for space */
} RTDX_inputChannel;

#define RTDX_INPUT_INITIAL_STATE    {0, 0, 0, 0, 0}

/* Macros to hide structure initialization data */
#define RTDX_CreateOutputChannel(c) \
        RTDX_outputChannel c = RTDX_OUTPUT_INITIAL_STATE
#define RTDX_CreateInputChannel(c)  \
        RTDX_inputChannel c = RTDX_INPUT_INITIAL_STATE


/* This is non-NULL if busy transmitting data                           */
extern void RTDX_DATA *volatile RTDX_writing;

/*************************************************************************
* Procedure:
*      RTDX_enableOutput
*      RTDX_disableOutput
*      RTDX_enableInput
*      RTDX_disableInput
* Parameters:
*      ochan   identifier for an output channel
*      ichan   identifier for an input  channel
*
* All input/output channels are initialized to be disabled.  A call to
* the input/output enable routine causes the specified channel to be
* enabled for data transmision.  Normally - user code will not call
* these functions.  Enabling/Disabling is typically done by the
* host-resident debugger.
*************************************************************************/
extern RTDX_CODE void RTDX_enableOutput ( RTDX_outputChannel *pOChan);
extern RTDX_CODE void RTDX_disableOutput( RTDX_outputChannel *pOChan);
extern RTDX_CODE void RTDX_enableInput  ( RTDX_inputChannel  *pIChan);
extern RTDX_CODE void RTDX_disableInput ( RTDX_inputChannel  *pIChan);

/* To convert function calls to macros where possible   */
#define RTDX_enableOutput(pOChan)   ( (pOChan)->enabled = RTDX_CHAN_ENABLED )
#define RTDX_disableOutput(pOChan)  ( (pOChan)->enabled = 0 )
#define RTDX_enableInput(pIChan)    { (pIChan)->enabled = RTDX_CHAN_ENABLED; \
                                      (pIChan)->busy = 0;}
#define RTDX_disableInput(pIChan)   ( (pIChan)->enabled = 0 )

#define RTDX_isInputEnabled(zzz) ((zzz)->enabled == RTDX_CHAN_ENABLED)
#define RTDX_isOutputEnabled(zzz) ((zzz)->enabled == RTDX_CHAN_ENABLED)

/* For Backward Compatability   */
#define RTDX_Enable_Data_Output     RTDX_enableOutput
#define RTDX_Disable_Data_Output    RTDX_disableOutput
#define RTDX_Enable_Data_Input      RTDX_enableInput
#define RTDX_Disable_Data_Input     RTDX_disableInput

/*************************************************************************
* Procedure:
*      RTDX_Init
*
* Effect:
*   Must be called to properly initialize RTDX.
*   Usually called as a result of pinit processing.
*   Calls RTDX_Buffer_Initialize & RTDX_Init_RT_Monitor.
*************************************************************************/
extern RTDX_CODE void RTDX_Init(void);

/*************************************************************************
* Procedure:
*      RTDX_Quit
*
* Effect:
*   Must be called to properly shutdown RTDX.
*   Usually called as a result of atexit processing.
*   Calls RTDX_Final_RT_Monitor
*************************************************************************/
extern RTDX_CODE void RTDX_Quit(void);

/*************************************************************************
* Procedure:
*      RTDX_write
* Parameters:
*      ochan   identifier for an output channel
*      buffer  a pointer to the buffer to contain the data
*      bsize   the size of the buffer in address units
* Returns:
*      int     Success ( non-zero ) / Fail (0)
*
* RTDX_write causes the specified data to be written to the specified
* output channel, provided the channel is enabled.  On return from
* RTDX_write, the data has been copied out of the specified buffer.
* If the channel is not enabled, RTDX_write is a no-op that returns success.
*************************************************************************/
extern RTDX_CODE int RTDX_write( 
    RTDX_outputChannel *pIChan, /* channel to write to                  */
    void *pvBuf,                /* pointer to buffer of data            */
    size_t cbBuf);              /* size of the buffer in address units  */
#define RTDX_Data_Write RTDX_write

/*************************************************************************
* Procedure:
*      RTDX_writeCB
*
* Parameters:
*      pOChan  identifier for an output channel
*      pvBuf   a pointer to the buffer to contain the data
*      cbBuf   the size of the buffer in address units
*      isdone  procedure to call when output is complete or space available
*
* Returns:
*   RTDX_WRITE_ERROR if the request is too larget to handle.
*   RTDX_CALLBACK_INUSE if the channel already has an outstanding request.
*   RTDX_FAILURE if the request cannot be posted due to the buffers being full.
*   RTDX_OK for transfer being successfully initialed.
*   RTDX_WRITE_DONE if the transfer has been completed, i.e. simulator, 
*                   or the channel is not enabled.
*
* RTDX_writeCB causes the specified data to be written to the specified
* output channel, provided the channel is enabled.  On successful return 
* from RTDX_writeCB, the data has been copied out of the specified buffer
* and the callback function will be called when the transfer is complete.
* On failure return, the callback will be called once when space is available.
* If a callback is already in place for this channel, an eror will be returned.
* If the channel is not enabled, RTDX_writeCB is a no-op that returns success.
*************************************************************************/
extern RTDX_CODE int RTDX_writeCB( 
    RTDX_outputChannel *pOChan, /* channel to write to                  */
    void *pvBuf,                /* pointer to buffer of data            */
    size_t cbBuf,               /* size of the buffer in address units  */
    RTDX_caction *isdone);      /* called when complete or space available */

/*************************************************************************
* Procedure:
*   RTDX_read
*
* Parameters:
*   ichan   identifier for the input channel to use
*   buffer  a pointer to the buffer to contain the data
*   bsize   the size of the buffer in address units
*
* Effect:
*   RTDX_read causes a read request to be posted to the specified input
* channel.  If the channel is enabled, RTDX_read then busy waits until the
* data has arrived. On return from RTDX_read, the data has been copied
* into the specified buffer.
*
* Returns:
*   RTDX_READ_ERROR if the channel is not enabled, or already busy.
* Otherwise, RTDX_read returns the actual number of units read, in
* sizeof() units.
*************************************************************************/
extern RTDX_CODE int RTDX_read( 
    RTDX_inputChannel *pIChan,  /* channel to read from                 */
    void *pvBuf,                /* pointer to buffer of data            */
    size_t cbBuf);              /* size of the buffer in address units  */
#define RTDX_Data_Read RTDX_read


/*************************************************************************
* Procedure:
*   RTDX_readNB
*
* Parameters:
*   ichan   identifier for the input channel to use
*   buffer  a pointer to the buffer to contain the data
*   bsize   the size of the buffer in address units
*
* Effect:
*   RTDX_readNB causes a read request to be posted to the specified
* input channel.  RTDX_readNB posts a read request to the host, and then
* immediately returns.  The user must wait for the data to arrive; a call
* to RTDX_channelBusy can be used to poll the channel for completion.
*
* Returns:
*   RTDX_READ_ERROR if the channel is not enabled, or busy.
*   0 if the request cannot be posted due to the buffers being full.
*   RTDX_OK for success.
*************************************************************************/
extern RTDX_CODE int RTDX_readNB( 
    RTDX_inputChannel *pIChan,  /* channel to read from                 */
    void *pvBuf,                /* pointer to buffer of data            */
    size_t cbBuf);              /* size of the buffer in address units  */
#define RTDX_Data_Read_NB RTDX_readNB


/*************************************************************************
* Procedure:
*      RTDX_readCB
*
* Parameters:
*      pIChan  identifier for an input channel
*      pvBuf   a pointer to the buffer to contain the data
*      cbBuf   the size of the buffer in address units
*      isdone  procedure to call when input is complete or space available
*
* Effect:
*   RTDX_readCB causes a read request to be posted to the specified
* input channel.  RTDX_readCB posts a read request to the host, and then
* immediately returns.  The user must wait for the data to arrive; the
* callback will be called when the read has completed.
*
* Returns:
*   RTDX_WRITE_ERROR if the channel is not enabled, or busy.
*   RTDX_FAILURE if the request cannot be posted due to the buffer being full.
*   RTDX_OK for transfer being successfully initialed.
*
* On RTDX_FAILURE return, the callback will be called once when space is available.
*
*************************************************************************/
extern RTDX_CODE int RTDX_readCB(
    RTDX_inputChannel *pIChan,   /* the input channel to use             */
    void *pvBuf,                 /* pointer to buffer of data            */
    size_t cbBuf,                /* size of the buffer in address units  */
    RTDX_caction *isdone);       /* called when complete or space available */


/*************************************************************************
* Procedure:
*      RTDX_channelBusy
*
* Parameters:
*      ichan   identifier for an input  channel
*
* Returns:
*      TRUE if the channel is currently busy awaiting input.  FALSE
* if it is not busy, meaning any previous request has completed.
*
* Notes:
*   Usually this function is an inline expanded macro.
*************************************************************************/
extern RTDX_CODE int RTDX_channelBusy( RTDX_inputChannel *pIChan );
#define RTDX_channelBusy( pIChan )     ( (pIChan)->busy )
#define RTDX_Channel_Busy RTDX_channelBusy


/*************************************************************************
* Procedure:
*      RTDX_sizeofInput
*
* Parameters:
*      ichan   identifier for an input  channel
*
* Returns:
*      The amount of data actually read by the last completed read
* operation, in sizeof() units, is returned.  The result of
* RTDX_sizeofInput is not valid if the channel is currently busy.
*
* Notes:
*   Usually this function is an inline expanded macro.
*************************************************************************/
extern RTDX_CODE size_t RTDX_sizeofInput( RTDX_inputChannel *pIChan );
#define RTDX_sizeofInput( pIChan )       ( (size_t)((pIChan)->count) )
#define RTDX_Bytes_Read RTDX_sizeofInput

/*************************************************************************
* Procedure:
*      RTDX_sizeofOutput
*
* Parameters:
*      None
*
* Returns:
*      The maximum amount of data that actually can be written
*************************************************************************/
extern RTDX_CODE size_t RTDX_sizeofOutput();

#ifdef __cplusplus
}
#endif

#endif  /* __RTDX_H */
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

