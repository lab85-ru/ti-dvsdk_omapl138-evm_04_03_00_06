/*
 * pros_edma3_drv_sample.h
 *
 * Header file for the sample application for the EDMA3 Driver.
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

#ifndef _PROS_EDMA3_DRV_SAMPLE_H_
#define _PROS_EDMA3_DRV_SAMPLE_H_

#include <stdio.h>

/* Include EDMA3 Driver */
#include <ti/sdo/edma3/drv/edma3_drv.h>

#ifdef __cplusplus
extern "C" {
#endif

/* To enable debug traces in the EDMA3 sample app */
#define EDMA3_DEBUG_PRINT

#ifdef EDMA3_DEBUG_PRINT
#include <stdio.h>
extern void kprintf ( char* fp, ... );
#define EDMA3_DEBUG_PRINTF           printf
#endif /* EDMA3_DEBUG_PRINT */


/* To include linking or chaining test case. */
#define QDMA_TEST_WITH_LINKING      (1u)
/* #undef QDMA_TEST_WITH_LINKING */

#define EDMA3_TEST_WITH_CHAINING    (1u)
/* #undef EDMA3_TEST_WITH_CHAINING */

/* To include Poll mode tests */
#define EDMA3_POLL_MODE_TEST        (1u)
/* #undef EDMA3_POLL_MODE_TEST */

/* To include ping-pong buffer tests */
#define EDMA3_PING_PONG_TEST        (1u)
/* #undef EDMA3_PING_PONG_TEST */


/**
 * Buffers (src and dest) are needed for mem-2-mem data transfers.
 * This define is for the MAXIMUM size and hence the maximum data
 * which could be transferred using the sample test cases below.
 */
#define MAX_BUFFER_SIZE                     (512u*32u*8u)


/**
 * Cache line size on the underlying SoC. It needs to be modified
 * for different cache line sizes, if the Cache is Enabled.
 */
#define EDMA3_CACHE_LINE_SIZE_IN_BYTES      (32u)


/* To enable/disable the cache .*/
#define EDMA3_ENABLE_DCACHE                 (1u)


/* OPT Field specific defines */
#define OPT_SYNCDIM_SHIFT                   (0x00000002u)
#define OPT_TCC_MASK                        (0x0003F000u)
#define OPT_TCC_SHIFT                       (0x0000000Cu)
#define OPT_ITCINTEN_SHIFT                  (0x00000015u)
#define OPT_TCINTEN_SHIFT                   (0x00000014u)


/* Error returned in case of data mismatch */
#define EDMA3_DATA_MISMATCH_ERROR           (-1)


/* Error returned in case of buffers are not aligned on the cache boundary */
#define EDMA3_NON_ALIGNED_BUFFERS_ERROR     (-2)


/**
 * \brief Semaphore Types
 *
 * Enums for different semaphore types
 */
typedef enum
{
  EDMA3_OS_SEMTYPE_FIFO         = 0,
  EDMA3_OS_SEMTYPE_PRIORITY     = 1
} EDMA3_OS_SemType;


/**
 * \brief Semaphore Attributes Data Structure
 *
 * Data structure for Semaphore Attributes
 */

typedef struct
{
  EDMA3_OS_SemType      sematr;		/**< Semaphore Attribute */
  unsigned int			isemcnt;	/**< Semaphore Count */
  unsigned int			maxsem;		/**< Max Semaphore */
}EDMA3_OS_SemAttrs;

/**
 * \brief SoC specific TC related information. Specified in the sample
 * configuration file (bios_edma3_sample_cfg.c).
 */
extern unsigned int numEdma3Tc;
extern unsigned int ccXferCompInt;
extern unsigned int ccErrorInt;
extern unsigned int tcErrorInt[8];

/**
 * \brief   EDMA3 Initialization
 *
 * This function initializes the EDMA3 Driver and registers the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Result edma3init (void);

/**
 * \brief   EDMA3 De-initialization
 *
 * This function removes the EDMA3 RM Instance and unregisters the
 * interrupt handlers. It also deletes the RM  Object.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
 EDMA3_DRV_Result edma3deinit (void);

/**
 *  \brief   Main sample test case which will call other EDMA3 test cases.
 *              If one wants to call Edma3 test cases, include this main
 *              test case only.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3MemToMemCpytest (void);




/**
 *  \brief   EDMA3 Cache Invalidate
 *
 *  This function invalidates the D cache.
 *
 *  \param  mem_start_ptr [IN]      Starting adress of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheInvalidate(unsigned int mem_start_ptr,
                           unsigned int num_bytes);



/**
 * \brief   EDMA3 Cache Flush
 *
 *  This function flushes (cleans) the Cache
 *
 *  \param  mem_start_ptr [IN]      Starting adress of memory.
 *                                  Please note that this should be
 *                                  aligned according to the cache line size.
 *  \param  num_bytes [IN]          length of buffer
 *  \return  EDMA3_DRV_SOK if success, else error code in case of error
 *          or non-alignment of buffers.
 *
 * Note: This function is required if the buffer is in DDR.
 * For other cases, where buffer is NOT in DDR, user
 * may or may not require the below implementation and
 * should modify it according to her need.
 */
EDMA3_DRV_Result Edma3_CacheFlush(unsigned int mem_start_ptr,
                      unsigned int num_bytes);



/**
  * Counting Semaphore related functions (OS dependent) should be
  * called/implemented by the application. A handle to the semaphore
  * is required while opening the driver/resource manager instance.
  */

/**
 * \brief   EDMA3 OS Semaphore Create
 *
 *      This function creates a counting semaphore with specified
 *      attributes and initial value. It should be used to create a semaphore
 *      with initial value as '1'. The semaphore is then passed by the user
 *      to the EDMA3 driver/RM for proper sharing of resources.
 * \param   initVal [IN] is initial value for semaphore
 * \param   attrs [IN] is the semaphore attributes ex: Fifo type
 * \param   hSem [OUT] is location to recieve the handle to just created
 *      semaphore
 * \return  EDMA3_DRV_SOK if succesful, else a suitable error code.
 */
EDMA3_DRV_Result edma3OsSemCreate(int initVal,
                           const EDMA3_OS_SemAttrs *attrs,
                           EDMA3_OS_Sem_Handle *hSem);



/**
 * \brief   EDMA3 OS Semaphore Delete
 *
 *      This function deletes or removes the specified semaphore
 *      from the system. Associated dynamically allocated memory
 *      if any is also freed up.
 * \warning OsSEM services run in client context and not in a thread
 *      of their own. If there exist threads pended on a semaphore
 *      that is being deleted, results are undefined.
 * \param   hSem [IN] handle to the semaphore to be deleted
 * \return  EDMA3_DRV_SOK if succesful else a suitable error code
 */
EDMA3_DRV_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* _PROS_EDMA3_DRV_SAMPLE_H_ */

