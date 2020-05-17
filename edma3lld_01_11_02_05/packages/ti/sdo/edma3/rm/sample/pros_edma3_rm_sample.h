/*
 * pros_edma3_rm_sample.h
 *
 * Header file for the Demo application for the EDMA3 Resource Manager.
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

#ifndef _PROS_EDMA3_RM_SAMPLE_H_
#define _PROS_EDMA3_RM_SAMPLE_H_

#include <ti/sdo/edma3/rm/edma3_rm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* To enable debug traces in the EDMA3 sample app */
#define EDMA3_DEBUG_PRINT

#ifdef EDMA3_DEBUG_PRINT
#include <stdio.h>
#define EDMA3_DEBUG_PRINTF           printf
#endif /* EDMA3_DEBUG_PRINT */

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

EDMA3_RM_Result edma3init();
EDMA3_RM_Result edma3deinit();

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
 *      to the EDMA3 RM for proper sharing of resources.
 * \param   initVal [IN] is initial value for semaphore
 * \param   attrs [IN] is the semaphore attributes ex: Fifo type
 * \param   hSem [OUT] is location to recieve the handle to just created
 *      semaphore
 * \return  EDMA3_RM_SOK if succesful, else a suitable error code.
 */
EDMA3_RM_Result edma3OsSemCreate(int initVal,
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
 * \return  EDMA3_RM_SOK if succesful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
