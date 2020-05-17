/*
 * pros_edma3_rm_sample_cs.c
 *
 * Sample functions showing the implementation of critical section entry/exit
 * routines and various semaphore related routines (all
 * OS depenedent). These implementations MUST be provided by the user /
 * application, using the EDMA3 Resource Manager, for its correct functioning.
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

#include <itron.h>
#include <ti/sdo/edma3/rm/sample/pros_edma3_rm_sample.h>
extern unsigned int Edma3_critsec32(void);
extern void Edma3_endcrit32(unsigned int intState);


/**
 * \brief   EDMA3 OS Protect Entry
 *
 *      This function saves the current state of protection in 'intState'
 *      variable passed by caller, if the protection level is
 *      EDMA3_OS_PROTECT_INTERRUPT. It then applies the requested level of
 *      protection.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is disabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, '*intState' specifies the
 *      Transfer Controller number whose interrupt needs to be disabled.
 *
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is memory location where current state of protection is
 *      saved for future use while restoring it via edma3OsProtectExit() (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectEntry (int level, unsigned int *intState)
    {
    if (((level == EDMA3_OS_PROTECT_INTERRUPT)
        || (level == EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR))
        && (intState == NULL))
        {
        return;
        }
    else
        {
        switch (level)
            {
            /* Disable all (global) interrupts */
            case EDMA3_OS_PROTECT_INTERRUPT :
        		*intState = Edma3_critsec32();
                break;

            /* Disable scheduler */
            case EDMA3_OS_PROTECT_SCHEDULER :
				dis_dsp();
                break;

            /* Disable EDMA3 transfer completion interrupt only */
            case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION :
                dis_int(ccXferCompInt);
                break;

            /* Disable EDMA3 CC error interrupt only */
            case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR :
                dis_int(ccErrorInt);
                break;

            /* Disable EDMA3 TC error interrupt only */
            case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR :
                switch (*intState)
                    {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        /* Fall through... */
                        /* Disable the corresponding interrupt */
                        dis_int(tcErrorInt[*intState]);
                        break;

                     default:
                        break;
                    }

                break;

            default:
                break;
            }
        }
    }


/**
 * \brief   EDMA3 OS Protect Exit
 *
 *      This function undoes the protection enforced to original state
 *      as is specified by the variable 'intState' passed, if the protection
 *      level is EDMA3_OS_PROTECT_INTERRUPT.
 *      For EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION and
 *      EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR, variable 'intState' is ignored,
 *      and the requested interrupt is enabled.
 *      For EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR, 'intState' specifies the
 *      Transfer Controller number whose interrupt needs to be enabled.
 * \param   level is numeric identifier of the desired degree of protection.
 * \param   intState is original state of protection at time when the
 *      corresponding edma3OsProtectEntry() was called (Only
 *      for EDMA3_OS_PROTECT_INTERRUPT protection level).
 * \return  None
 */
void edma3OsProtectExit (int level, unsigned int intState)
    {
    switch (level)
        {
        /* Enable all (global) interrupts */
        case EDMA3_OS_PROTECT_INTERRUPT :
            Edma3_endcrit32(intState);
            break;

        /* Enable scheduler */
        case EDMA3_OS_PROTECT_SCHEDULER :
			ena_dsp();
            break;

        /* Enable EDMA3 transfer completion interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_XFER_COMPLETION :
            ena_int(ccXferCompInt);
            break;

        /* Enable EDMA3 CC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_CC_ERROR :
            ena_int(ccErrorInt);
            break;

        /* Enable EDMA3 TC error interrupt only */
        case EDMA3_OS_PROTECT_INTERRUPT_TC_ERROR :
            switch (intState)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    /* Fall through... */
                    /* Enable the corresponding interrupt */
                    ena_int(tcErrorInt[intState]);
                    break;

                 default:
                    break;
                }

            break;

        default:
            break;
        }
    }


/* Function to wait for OS Ticks */
void edma3OsWaitMsecs(unsigned int mSecs)
    {
    unsigned int ticks=0;
	static unsigned int edma3ProsMSecsPerOsTick = 10u; /* By default 1 mSecs per OS tick */
	EDMA3_RM_Result retVal = EDMA3_RM_SOK;

    if ((mSecs <= edma3ProsMSecsPerOsTick) == TRUE)
        {
        ticks = 1u;
        }
    else
        {
        ticks = (mSecs / edma3ProsMSecsPerOsTick);
        if ((mSecs % edma3ProsMSecsPerOsTick) > 0)
            {
            ++ticks;
            }
        }

	if (E_OK == dly_tsk((TMO)ticks))
        {
		retVal = EDMA3_RM_SOK;
	    }
	else
        {
		retVal = EDMA3_RM_E_INVALID_PARAM;
	    }

	retVal = retVal;
    }




/**
  * Counting Semaphore related functions (OS dependent) should be
  * called/implemented by the application. A handle to the semaphore
  * is required while opening the resource manager instance.
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
                           EDMA3_OS_Sem_Handle *hSem)
    {
    EDMA3_RM_Result semCreateResult = EDMA3_RM_SOK;

    if((NULL == hSem) || (NULL == attrs))
        {
        semCreateResult = EDMA3_RM_E_INVALID_PARAM;
        }
    else
        {
		*hSem = (EDMA3_OS_Sem_Handle) acre_sem((T_CSEM*)attrs);
        if ( (*hSem) == (EDMA3_OS_Sem_Handle) E_NOID )
            {
            semCreateResult = EDMA3_RM_E_SEMAPHORE;
            }
        }

    return semCreateResult;
    }


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
EDMA3_RM_Result edma3OsSemDelete(EDMA3_OS_Sem_Handle hSem)
    {
    EDMA3_RM_Result semDeleteResult = EDMA3_RM_SOK;

    if(NULL == hSem)
        {
        semDeleteResult = EDMA3_RM_E_INVALID_PARAM;
        }
    else
        {
        semDeleteResult = (EDMA3_RM_Result) del_sem((ID) hSem);
		if(semDeleteResult != (EDMA3_RM_Result) E_OK)
            {
			semDeleteResult = EDMA3_RM_E_SEMAPHORE;
			}
        }

    return semDeleteResult;
    }


/**
 * \brief   EDMA3 OS Semaphore Take
 *
 *      This function takes a semaphore token if available.
 *      If a semaphore is unavailable, it blocks currently
 *      running thread in wait (for specified duration) for
 *      a free semaphore.
 * \param   hSem [IN] is the handle of the specified semaphore
 * \param   mSecTimeout [IN] is wait time in milliseconds
 * \return  EDMA3_RM_Result if successful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemTake(EDMA3_OS_Sem_Handle hSem, int mSecTimeout)
    {
    EDMA3_RM_Result semTakeResult = EDMA3_RM_SOK;
    unsigned short semPendResult;

    if(NULL == hSem)
        {
        semTakeResult = EDMA3_RM_E_INVALID_PARAM;
        }
    else
        {
		semPendResult = (EDMA3_RM_Result) twai_sem((ID) hSem, mSecTimeout);
        if (semPendResult != (unsigned short) E_OK)
            {
            semTakeResult = EDMA3_RM_E_SEMAPHORE;
            }
        }

    return semTakeResult;
    }


/**
 * \brief   EDMA3 OS Semaphore Give
 *
 *      This function gives or relinquishes an already
 *      acquired semaphore token
 * \param   hSem [IN] is the handle of the specified semaphore
 * \return  EDMA3_RM_Result if successful else a suitable error code
 */
EDMA3_RM_Result edma3OsSemGive(EDMA3_OS_Sem_Handle hSem)
    {
    EDMA3_RM_Result semGiveResult = EDMA3_RM_SOK;

    if(NULL == hSem)
        {
        semGiveResult = EDMA3_RM_E_INVALID_PARAM;
        }
    else
        {
        semGiveResult = (EDMA3_RM_Result) sig_sem((ID) hSem);
		if(semGiveResult != (EDMA3_RM_Result) E_OK)	{
			semGiveResult = EDMA3_RM_E_SEMAPHORE;
			}
        }

    return semGiveResult;
    }
