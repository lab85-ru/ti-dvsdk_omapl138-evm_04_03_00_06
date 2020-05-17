/*
 * bios_edma3_drv_sample_init.c
 *
 * Sample Initialization for the EDMA3 Driver (for single-DSP based devices) for
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

#include <ecm.h>
#include <hwi.h>

#include <ti/sdo/edma3/drv/sample/bios_edma3_drv_sample.h>


/** @brief To enable the HWI event corresponding to the EDMA3
 * transfer completion ECM event.
 */
#define EDMA3_HWI_BITMASK_XFER_COMP			(1u << hwIntXferComp)

/** @brief To enable the HWI event corresponding to the EDMA3
 * channel controller error ECM event.
 */
#define EDMA3_HWI_BITMASK_CC_ERR			(1u << hwIntCcErr)

/** @brief To enable the HWI event corresponding to the EDMA3
 * transfer controller error ECM event.
 */
#define EDMA3_HWI_BITMASK_TC_ERR			(1u << hwIntTcErr)

/** @brief EDMA3 Driver Handle, used to call all the Driver APIs */
EDMA3_DRV_Handle hEdma = NULL;

/** @brief EDMA3 Driver Instance specific Semaphore handle */
EDMA3_OS_Sem_Handle semHandle = NULL;


/**
  * EDMA3 TC ISRs which need to be registered with the underlying OS by the user
  * (Not all TC error ISRs need to be registered, register only for the
  * available Transfer Controllers).
  */
void (*ptrEdma3TcIsrHandler[EDMA3_MAX_TC])(unsigned int arg) =
                                                {
                                                &lisrEdma3TC0ErrHandler0,
                                                &lisrEdma3TC1ErrHandler0,
                                                &lisrEdma3TC2ErrHandler0,
                                                &lisrEdma3TC3ErrHandler0,
                                                &lisrEdma3TC4ErrHandler0,
                                                &lisrEdma3TC5ErrHandler0,
                                                &lisrEdma3TC6ErrHandler0,
                                                &lisrEdma3TC7ErrHandler0,
                                                };


signed char*  getGlobalAddr(signed char* addr)
{
     return (addr); /* The address is already a global address */
}

/**  To Register the ISRs with the underlying OS, if required. */
void registerEdma3Interrupts (void);
/**  To Unregister the ISRs with the underlying OS, if previously registered. */
void unregisterEdma3Interrupts (void);

/*
 *  ======== C64_enableIER ========
 *  enable interrupts specified by mask
 */
extern Void C64_enableIER( Uns mask );


/* External Global Configuration Structure */
extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams;

/* External Instance Specific Configuration Structure */
extern EDMA3_DRV_InstanceInitConfig sampleInstInitConfig;

/**
 * \brief SoC specific TC related information. Specified in the sample
 * configuration file (bios_edma3_sample_cfg.c).
 */
extern unsigned int numEdma3Tc;
extern unsigned int ccXferCompInt;
extern unsigned int ccErrorInt;
extern unsigned int tcErrorInt[8];
extern unsigned int hwIntXferComp;
extern unsigned int hwIntCcErr;
extern unsigned int hwIntTcErr;

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
    unsigned int edma3InstanceId = 0u;
    EDMA3_DRV_InitConfig initCfg;
    EDMA3_DRV_Result    edma3Result = EDMA3_DRV_SOK;
    EDMA3_OS_SemAttrs semAttrs = {EDMA3_OS_SEMTYPE_FIFO, NULL};
    EDMA3_DRV_GblConfigParams *globalConfig = &sampleEdma3GblCfgParams;
    EDMA3_DRV_InstanceInitConfig *instanceConfig = &sampleInstInitConfig;
    EDMA3_RM_MiscParam miscParam;

    if (NULL == hEdma)
        {
        /* configuration structure for the Driver */
        initCfg.isMaster    =    TRUE;
        initCfg.regionId = (EDMA3_RM_RegionId)1u;
        initCfg.drvSemHandle = NULL;
        /* Driver instance specific config NULL */
        initCfg.drvInstInitConfig = instanceConfig;
        initCfg.gblerrCb = NULL;
        initCfg.gblerrData = NULL;

        miscParam.isSlave = FALSE;

        /* Create EDMA3 Driver Object first. */
        edma3Result = EDMA3_DRV_create (edma3InstanceId, globalConfig ,
        									(void *)&miscParam);

        if (edma3Result != EDMA3_DRV_SOK)
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3init: EDMA3_DRV_create FAILED\r\n");
#endif
            }
        else
            {
            /**
              * Driver Object created successfully.
              * Create a semaphore now for driver instance.
              */
            edma3Result = edma3OsSemCreate(1, &semAttrs, &initCfg.drvSemHandle);
            if (edma3Result != EDMA3_DRV_SOK)
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3init: edma3OsSemCreate FAILED\r\n");
#endif
                }
            else
                {
                /* Save the semaphore handle for future use */
                semHandle = initCfg.drvSemHandle;

                /* Open the Driver Instance */
                hEdma = EDMA3_DRV_open (edma3InstanceId, (void *) &initCfg,
                                        &edma3Result);
                if(NULL == hEdma)
                    {
#ifdef EDMA3_DRV_DEBUG
                    EDMA3_DRV_PRINTF("edma3init: EDMA3_DRV_open FAILED\r\n");
#endif
                    }
                else
                    {
                    /**
                     * Register Interrupt Handlers for various interrupts
                     * like transfer completion interrupt, CC error
                     * interrupt, TC error interrupts etc, if required.
                     */
                    registerEdma3Interrupts();
                    }
                }
            }
        }
    else
        {
        /* EDMA3 Driver already initialized, no need to do that again. */
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3init: EDMA3 Driver Already Initialized...Init failed\r\n");
#endif
        edma3Result = EDMA3_DRV_E_INVALID_STATE;
        }

     return edma3Result;
    }


/**  To Register the ISRs with the underlying OS, if required. */
void registerEdma3Interrupts (void)
    {
    unsigned int intState;
    ECM_Attrs ecmattrs = ECM_ATTRS;
    unsigned int numTc = 0;

    /* Disabling the global interrupts */
    intState = HWI_disable();

    /* Enable the Xfer Completion Event Interrupt */
    ecmattrs.unmask = 1u;
	ecmattrs.arg = (Arg)0u;

    ECM_dispatchPlug (ccXferCompInt, (ECM_Fxn)(&lisrEdma3ComplHandler0),
                        &ecmattrs);
    ECM_enableEvent(ccXferCompInt);

    /* Enable the CC Error Event Interrupt */
    ecmattrs.unmask = 1u;
	ecmattrs.arg = (Arg)0u;

    ECM_dispatchPlug(ccErrorInt, (ECM_Fxn)(&lisrEdma3CCErrHandler0), &ecmattrs);
    ECM_enableEvent(ccErrorInt);

    /* Enable the TC Error Event Interrupt, according to the number of TCs. */
    while (numTc < numEdma3Tc)
    {
        ecmattrs.unmask = 1u;
		ecmattrs.arg = (Arg)0u;

        ECM_dispatchPlug (tcErrorInt[numTc],
                            (ECM_Fxn)(ptrEdma3TcIsrHandler[numTc]),
                            &ecmattrs);
        ECM_enableEvent(tcErrorInt[numTc]);
        numTc++;
    }


   /**
    * Enabling the HWI_ID.
    * EDMA3 interrupts (transfer completion, CC error etc.)
    * correspond to different ECM events (SoC specific). These ECM events come
    * under ECM block XXX (handling those specific ECM events). Normally, block
    * 0 handles events 4-31 (events 0-3 are reserved), block 1 handles events
    * 32-63 and so on. This ECM block XXX (or interrupt selection number XXX)
    * is mapped to a specific HWI_INT YYY in the tcf file. So to enable this
    * mapped HWI_INT YYY, one should use the corresponding bitmask in the
    * API C64_enableIER(), in which the YYY bit is SET.
    */
    /* To enable EDMA3 transfer completion interrupt */
	C64_enableIER(EDMA3_HWI_BITMASK_XFER_COMP);
	/* To enable EDMA3 CC Error interrupt */
	C64_enableIER(EDMA3_HWI_BITMASK_CC_ERR);
	/* To enable EDMA3 TC Error interrupt */
    C64_enableIER(EDMA3_HWI_BITMASK_TC_ERR);

    /* Restore interrupts */
    HWI_restore(intState);
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
    EDMA3_DRV_Result    edma3Result = EDMA3_DRV_SOK;

    /* Unregister Interrupt Handlers first */
    unregisterEdma3Interrupts();

    /* Delete the semaphore */
    edma3Result = edma3OsSemDelete(semHandle);
    if (EDMA3_DRV_SOK != edma3Result )
        {
#ifdef EDMA3_DRV_DEBUG
        EDMA3_DRV_PRINTF("edma3deinit: edma3OsSemDelete FAILED\r\n");
#endif
        }
    else
        {
        /* Make the semaphore handle as NULL. */
        semHandle = NULL;

        /* Now, close the EDMA3 Driver Instance */
        edma3Result = EDMA3_DRV_close (hEdma, NULL);
        if (EDMA3_DRV_SOK != edma3Result )
            {
#ifdef EDMA3_DRV_DEBUG
            EDMA3_DRV_PRINTF("edma3deinit: EDMA3_DRV_close FAILED\r\n");
#endif
            }
        else
            {
            /* Make the Drv handle as NULL. */
            hEdma = NULL;

            /* Now, delete the EDMA3 Driver Object */
            edma3Result = EDMA3_DRV_delete (edmaInstanceId, NULL);
            if (EDMA3_DRV_SOK != edma3Result )
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3deinit: EDMA3_DRV_delete FAILED\r\n");
#endif
                }
            else
                {
#ifdef EDMA3_DRV_DEBUG
                EDMA3_DRV_PRINTF("edma3deinit: EDMA3 Deinitialization" \
                                    " Completed...\r\n");
#endif
                }
            }
        }

    return edma3Result;
    }


/**  To Unregister the ISRs with the underlying OS, if previously registered. */
void unregisterEdma3Interrupts (void)
    {
    unsigned int intState;
    unsigned int numTc = 0;

    /* Disabling the global interrupts */
    intState = HWI_disable();

    /* Disable the Xfer Completion Event Interrupt */
    ECM_disableEvent(ccXferCompInt);

    /* Disable the CC Error Event Interrupt */
    ECM_disableEvent(ccErrorInt);

    /* Enable the TC Error Event Interrupt, according to the number of TCs. */
    while (numTc < numEdma3Tc)
    {
        ECM_disableEvent(tcErrorInt[numTc]);
        numTc++;
    }

    /* Restore interrupts */
    HWI_restore(intState);
    }

/* End of File */
