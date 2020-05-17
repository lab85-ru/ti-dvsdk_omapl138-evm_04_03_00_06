/*
 * bios_edma3_drv_sample_init_multi_dsp.c
 *
 * Sample Initialization for the EDMA3 Driver (for multi-DSP based devices) for
 * BIOS 5 based applications. It should be MANDATORILY done once before EDMA3
 * usage.
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

#include <ti/sdo/edma3/drv/sample/bios_edma3_drv_sample.h>


/** @brief EDMA3 Driver Handle, used to call all the Driver APIs */
EDMA3_DRV_Handle hEdma = NULL;

/** @brief EDMA3 Driver Instance specific Semaphore handle */
EDMA3_OS_Sem_Handle semHandle = NULL;

/**  To Register the ISRs with the underlying OS, if required. */
void registerEdma3Interrupts (void);
/**  To Unregister the ISRs with the underlying OS, if previously registered. */
void unregisterEdma3Interrupts (void);

/* External Global Configuration Structure */
extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams;

/* External Instance Specific Configuration Structure */
extern EDMA3_DRV_InstanceInitConfig sampleInstInitConfig[];

extern volatile cregister unsigned DNUM;

#define MAP_LOCAL_TO_GLOBAL_ADDR(addr) ((1<<28)|(DNUM<<24)|(((unsigned int)addr)&0x00ffffff))

/* Determine the processor id by reading DNUM register. */
unsigned short determineProcId()
{

    return (DNUM);
}

signed char*  getGlobalAddr(signed char* addr)
{
    if (((unsigned int)addr & (unsigned int)0xFF000000) != 0)
    {
        return (addr); /* The address is already a global address */
    }

    return((signed char*)(MAP_LOCAL_TO_GLOBAL_ADDR(addr)));
}


/** Whether global configuration required for EDMA3 or not.
 * This configuration should be done only once for the EDMA3 hardware by
 * any of the masters. Specified in the sample configuration file.
 */
extern unsigned int gblCfgReqdArray [];

/**
 * DSP instance number on which the executable is running. Its value is
 * determined by reading the processor specific register DNUM.
 */
unsigned int dsp_num;

/**
 * Variable array which will be used internally for referring to
 * transfer completion interrupts for different DSPs.
 */
extern unsigned int ccXferCompInt[];

/**
 * \brief   EDMA3 Initialization
 *
 * This function initializes the EDMA3 Driver and registers the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
 EDMA3_DRV_Result edma3init ()
    {
    unsigned int edma3InstanceId = 0;
    EDMA3_DRV_InitConfig initCfg;
    EDMA3_DRV_Result edma3Result = EDMA3_DRV_SOK;
    EDMA3_OS_SemAttrs semAttrs = {EDMA3_OS_SEMTYPE_FIFO, NULL};
    EDMA3_DRV_GblConfigParams *globalConfig = &sampleEdma3GblCfgParams;
    EDMA3_DRV_InstanceInitConfig *instanceConfig = NULL;
    EDMA3_DRV_MiscParam miscParam;

    /* DSP instance number */
    dsp_num = determineProcId();

    if (NULL == hEdma)
        {
		/**
		 * Required for devices where global configuration is done by some other
		 * core (like ARM).
		 * If DSP is doing the initialization, configure it as 'Master'.
		 */
        miscParam.isSlave = gblCfgReqdArray[dsp_num];

        /** Create EDMA3 Driver Object first.
         * Before that, change the transfer completion interrupt number in the
         * global configuration structure according to the DSP instance number.
         */
		globalConfig->xferCompleteInt = ccXferCompInt[dsp_num];
        edma3Result = EDMA3_DRV_create (edma3InstanceId, globalConfig ,
        									(void *)&miscParam);
		}
    else
        {
        /* EDMA3 Driver already initialized, no need to do that again. */
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3init: EDMA3 Driver Already Initialized...Init failed\r\n");
#endif
        edma3Result = EDMA3_DRV_E_INVALID_STATE;
        }

    if (edma3Result == EDMA3_DRV_SOK)
        {
        /**
          * Driver Object created successfully.
          * Create a semaphore now for driver instance.
          */
        edma3Result = edma3OsSemCreate(1, &semAttrs, &initCfg.drvSemHandle);
    	}

    if (edma3Result == EDMA3_DRV_SOK)
        {
        /* Save the semaphore handle for future use */
        semHandle = initCfg.drvSemHandle;

        /* configuration structure for the Driver */
        initCfg.isMaster = TRUE;	/* Required to receive interrupts */
        initCfg.gblerrCb = NULL;
        initCfg.gblerrData = NULL;

		/**
		 * Some parameters depend on the DSP instance number.
		 * Hence, find the DSP number first and then configure
		 * EDMA3 Driver Instance accordingly.
		 */
		/* Region id is same as DSP number */
		initCfg.regionId = (EDMA3_RM_RegionId)dsp_num;

		/** Pick Driver Instance specific configuration according to the DSP
		* number.
		*/
		instanceConfig = &sampleInstInitConfig[dsp_num];

		/* Save it in the configuration structure. */
		initCfg.drvInstInitConfig = instanceConfig;


        /* Open the Driver Instance */
        hEdma = EDMA3_DRV_open (edma3InstanceId, (void *) &initCfg, &edma3Result);
		}

    if((NULL != hEdma) && (edma3Result == EDMA3_DRV_SOK))
        {
        /**
         * Register Interrupt Handlers for various interrupts
         * like transfer completion interrupt, CC error
         * interrupt, TC error interrupts etc, if required.
         */
        registerEdma3Interrupts();
        }

	return edma3Result;
    }

/**
 * \brief   EDMA3 De-initialization
 *
 * This function removes the EDMA3 Driver instance and unregisters the
 * interrupt handlers.
 *
  * \return  EDMA3_DRV_SOK if success, else error code
 */
EDMA3_DRV_Result edma3deinit (void)
	{
    unsigned int edmaInstanceId = 0;
    EDMA3_DRV_Result edma3Result = EDMA3_DRV_SOK;

    /* Unregister Interrupt Handlers first */
    unregisterEdma3Interrupts();

    /* Delete the semaphore */
    edma3Result = edma3OsSemDelete(semHandle);

    if (EDMA3_DRV_SOK == edma3Result)
        {
        /* Make the semaphore handle as NULL. */
        semHandle = NULL;

        /* Now, close the EDMA3 Driver Instance */
        edma3Result = EDMA3_DRV_close (hEdma, NULL);
    	}

    if (EDMA3_DRV_SOK == edma3Result )
        {
        /* Make the Drv handle as NULL. */
        hEdma = NULL;

        /* Now, delete the EDMA3 Driver Object */
        edma3Result = EDMA3_DRV_delete (edmaInstanceId, NULL);
    	}

    return edma3Result;
    }

/* End of File */
