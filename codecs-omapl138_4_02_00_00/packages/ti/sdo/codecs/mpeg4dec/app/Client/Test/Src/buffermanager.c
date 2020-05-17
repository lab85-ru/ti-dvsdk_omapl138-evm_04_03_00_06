/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/
/** @mainpage Mpeg2 Decoder
*
* @section Introduction
*
* @subsection xxx Purpose and Scope
* The purpose of this document is to detail the  Mpeg2 APIs
*
*
*/

/*!
********************************************************************************
@file     BufferManager.c
@brief    This is source module for buffer manager functionality
(Baseline Profile) Video Encoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.0 - May 17,2007    initial version
********************************************************************************
*/

/* Standard C header files */
//#include <tistdtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <std.h>
#include <xdas.h>
#ifdef MSVC
#include <xdm.h>
#else
#include <ti/xdais/dm/xdm.h>
#endif
#include "buffermanager.h"

#define MAX_DPB_LUMA_SIZE   (10705920)
#define MAX_DPB_CHROMA_SIZE (5352960)

BUFFMGR_buffEle buffArray[MAX_BUFF_ELEMENTS];

/*****************************************************************************/
/**
*@func BUFFMGR_Init()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       initialization module
*
*        The BUFFMGR_Init function is called by the test application to
*        initialise the global buffer element array to default and to allocate
*        required number of memory data for reference and output buffers.
*        The maximum required dpb size is defined by the supported profile &
*        level.
*
*@param  numBufs
*        Number of buffers to be allocated
*
*@param  bufSizes
*        Address of array containing the buffer sizes of the numbufs
*
*@return Success(0)/failure(-1) in allocating and initialising
*
*/
/*****************************************************************************/
XDAS_Int32 BUFFMGR_Init(XDAS_Int32 numBufs, XDAS_Int32 *bufSizes)
{
    XDAS_UInt32 tmpCnt;
    XDAS_UInt8  idx;

    XDAS_UInt8 *tmpBufPtr;

    /* Initialise the elements in the global buffer array */
    for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
    {
        buffArray[tmpCnt].bufId      = tmpCnt+1;
        buffArray[tmpCnt].bufStatus  = BUFFMGR_BUFFER_FREE;

       for (idx = 0; idx < numBufs; idx++)
        {
            buffArray[tmpCnt].bufSize[idx] = 0;
            buffArray[tmpCnt].buf[idx]     = NULL;
        }
     }

    for(idx = 0; idx < numBufs; idx++)
    {
       tmpBufPtr = malloc (bufSizes[idx]);

       /* Check if allocation took place properly or not */
       if (tmpBufPtr == NULL)
       {
           free ((XDAS_UInt8 *)tmpBufPtr);
           return -1;
       }

        buffArray[0].buf[idx]     = tmpBufPtr;
        buffArray[0].bufSize[idx] = bufSizes[idx];
    }
     return 0;
}


/****************************************************************************/
/**
*@func BUFFMGR_ReInit()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*       The BUFFMGR_ReInit function allocates global luma and chroma buffers
*       and allocates entire space to first element. This element will be used
*       in first frame decode. After the picture's height and width and its
*       luma and chroma buffer requirements are obtained the global luma and
*       chroma buffers are re-initialised to other elements in teh buffer
*       array.
*
*@param  numBufs
*        Number of buffers to be allocated
*
*@param  bufSizes
*        Address of array containing the buffer sizes of the numbufs
*
*@return Success(0)/failure(-1) in allocating and initialising
*
*/
/****************************************************************************/

XDAS_Int32 BUFFMGR_ReInit
(
   XDAS_Int32 numRefBufs,
   XDAS_Int32 numOutBufs,
   XDAS_Int32 *bufSizes
)
{
    XDAS_UInt8 *tmpBufPtr;
    XDAS_UInt32 idx, ctr;

    /* As the application would have already provided one buffer    */
    /* allocate memory for 1 minus the actual number of reference   */
    /* buffers.                                                     */

    for(idx = 1; idx < numRefBufs; idx++)
    {
        for(ctr = 0; ctr < numOutBufs; ctr++)
        {
           tmpBufPtr = malloc (bufSizes[ctr]);

           if(tmpBufPtr == NULL)
           {
              free(tmpBufPtr);
              return -1;
           }

           buffArray[idx].buf[ctr]     = tmpBufPtr;
           buffArray[idx].bufSize[ctr] = bufSizes[ctr];

        }
    }

    return 0;
}


/*****************************************************************************/
/**
*@func BUFFMGR_GetFreeBuffer()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*        The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
*        global buffer array and returns the address of that element. Incase
*        if none of the elements are free then it returns NULL
*
*
*@return Valid buffer element address or NULL incase if no buffers are empty
*
*/
/*****************************************************************************/
BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer()
{
    XDAS_UInt32 tmpCnt;
    for(tmpCnt = 0;
        tmpCnt < MAX_BUFF_ELEMENTS;
        tmpCnt++)
    {
        /* Check for first empty buffer in the array and return its address */
        if(buffArray[tmpCnt].bufStatus == BUFFMGR_BUFFER_FREE)
        {
            buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_USED;
            return (&buffArray[tmpCnt]); 
        }
    }
    /* Incase if no elements in the array are free then return NULL */
    return NULL;
}


/*****************************************************************************/
/**
*@func BUFFMGR_ReleaseBuffer(buffId)
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*        The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
*        which are released by the test-app. "0" is not a valid buffer Id
*        hence this function keeps moving until it encounters a buffer Id
*        as zero or it hits the MAX_BUFF_ELEMENTS
*
*
*@return None
*
*/
/*****************************************************************************/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[])
{
    XDAS_UInt32 tmpCnt, tmpId;
    for(tmpCnt = 0;
        (tmpCnt < MAX_BUFF_ELEMENTS);
        tmpCnt++)
    {
        tmpId = bufffId[tmpCnt];
        /*
         * Check if the buffer Id = 0 condition has reached. zero is not a
         * valid buffer Id hence that value is used to identify the end of
         * buffer array
         */
        if(tmpId == 0)
        {
           break;
        }
        /*
         * convert the buffer-Id to its corresponding index in the global
         * array
         */
        tmpId--;

        /* Set the status of the buffer to FREE */
        buffArray[tmpId].bufStatus = BUFFMGR_BUFFER_FREE;
     
    }
    return;
}

/****************************************************************************/
/**
*@func BUFFMGR_ReleaseAllBuffers()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*       The BUFFMGR_ReleaseAllBuffers function will set the status of
*       all buffer elements in the buffArray[] to free. This is called
*       for cases when application does not set freebufid of all buffers
*       typically when stream has not finished but app wants to end decoding.
*
*@return None
*/
/****************************************************************************/
void BUFFMGR_ReleaseAllBuffers()
{
    XDAS_UInt32 tmpCnt;
    for(tmpCnt = 0;
        (tmpCnt < MAX_BUFF_ELEMENTS);
        tmpCnt++)
    {
        /* Set the status of the buffer to FREE */
        buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;
    }
    return;
}
/****************************************************************************/
/**
*@func BUFFMGR_DeInit()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*        The BUFFMGR_DeInit function releases all memory allocated by buffer
*           manager.
*
*@param  numBufs
*        Number of buffers to be de-allocated
*
*@return None
*
*/
/****************************************************************************/

void BUFFMGR_DeInit
(
   XDAS_Int32 numRefBufs,
   XDAS_Int32 numOutBufs
)
{
    XDAS_UInt32 idx,ctr;

    /* As the application would have already provided one buffer    */
    /* allocate memory for 1 minus the actual number of reference   */
    /* buffers.                                                     */

    for(idx = 0; idx < numRefBufs; idx++)
    {
        for(ctr = 0; ctr < numOutBufs; ctr++)
        {
           free((void *)buffArray[idx].buf[ctr]);
           buffArray[idx].buf[ctr] = NULL;
        }
    }

    return;

}
