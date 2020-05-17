/*
 * sata_startup.c
 *
 * This file contains the startup code for sata driver.
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

#include <std.h>
#include <iom.h>

#include "ti/pspiom/sata/psp_sata.h"
#include "ti/pspiom/blkmedia/psp_blkdev.h"
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/cslr/cslr_psc_C6748.h"
#include "ti/pspiom/platforms/evm6748/sata_evmInit.h"
#include "ti/pspiom/cslr/cslr_sata.h"
#include "ti/sdo/edma3/drv/edma3_drv.h"      /* EDMA3 defines */
#include "ti/pspiom/sata/psp_ata_pwrm.h"

#define SATA_INST_ID                 0U 		/* Instance no of SATA AHCI */
#define SATA_INTR_GROUP				 (1<<8)     /* InterruptGroup */

#define	PINMUX_FN_SATA_LED			3
#define	PINMUX_FN_SATA_CP_POD		3
#define	PINMUX_FN_SATA_MP_SWITCH 	4
#define	PINMUX_FN_SATA_CP_DET		4
#define	POWER_CTRL_POWER_UP			1

#define SATACTRL_PWRDN					(0x01E2C018)

/* Port PHY control defintions */
#define SATA_PHYCR_ENPLL 				(1 << CSL_SATA_P0PHYCR_ENPLL_SHIFT)    
#define SATA_PHYCR_OVERRI 				(0 << CSL_SATA_P0PHYCR_OVERRI_SHIFT)    
#define SATA_PHYCR_TXDE 				(0 << CSL_SATA_P0PHYCR_TXDE_SHIFT)      
#define SATA_PHYCR_TXSWING 				(3 << CSL_SATA_P0PHYCR_TXSWING_SHIFT)   
#define SATA_PHYCR_TXCM 				(0 << CSL_SATA_P0PHYCR_TXCM_SHIFT)      
#define SATA_PHYCR_TXINVPAIR 			(0 << CSL_SATA_P0PHYCR_TXINVPAIR_SHIFT) 
#define SATA_PHYCR_RXEQ				    (1 << CSL_SATA_P0PHYCR_RXEQ_SHIFT)      

#define SATA_PHYCR_RXCDR_0				(0 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_1				(1 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_2				(2 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_3				(3 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_4				(4 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_5				(5 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_6				(6 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     
#define SATA_PHYCR_RXCDR_7				(7 << CSL_SATA_P0PHYCR_RXCDR_SHIFT)     

#define SATA_PHYCR_RXTERM_VSSA			(0 << CSL_SATA_P0PHYCR_RXTERM_SHIFT)    
#define SATA_PHYCR_RXTERM_VDDA1			(1 << CSL_SATA_P0PHYCR_RXTERM_SHIFT)    
#define SATA_PHYCR_RXTERM_VDDA2			(2 << CSL_SATA_P0PHYCR_RXTERM_SHIFT)    
#define SATA_PHYCR_RXTERM_FLOAT			(3 << CSL_SATA_P0PHYCR_RXTERM_SHIFT)    

#define SATA_PHYCR_RXINVPAIR			(0 << CSL_SATA_P0PHYCR_RXINVPAIR_SHIFT)
#define SATA_PHYCR_STD_OSCSEL_DIS		(0 << CSL_SATA_P0PHYCR_LOS_SHIFT)
#define SATA_PHYCR_STD_OSCSEL_EN		(1 << CSL_SATA_P0PHYCR_LOS_SHIFT)
#define SATA_PHYCR_MED_BAND_WIDTH		(0 << CSL_SATA_P0PHYCR_LB_SHIFT)
#define SATA_PHYCR_ULTR_BAND_WIDTH		(1 << CSL_SATA_P0PHYCR_LB_SHIFT)
#define SATA_PHYCR_LOW_BAND_WIDTH		(2 << CSL_SATA_P0PHYCR_LB_SHIFT)
#define SATA_PHYCR_HIGH_BAND_WIDTH		(3 << CSL_SATA_P0PHYCR_LB_SHIFT)
#define SATA_PHYCR_MPY_VAL				(8 << CSL_SATA_P0PHYCR_MPY_SHIFT)

/**
 * \brief gFreonSataRes
 *
 * platform resource
 */
PlatformResource SataRes[] =
{
	CSL_SATA_PER_CNT,
	{
		CSL_SATA_0_REGS,				/* base address of sata controller */
		CSL_INTC_EVENTID_SATAINT,		/* hardware interrupt number */
		SATA_INTR_GROUP					/* event group */
	},
	SATA_PSC_PWRM_DISABLE, 				/* pscPwrmEnable set to zero */
#ifdef BIOS_PWRM_ENABLE
	PWRM_CPU
#endif
};

/**
 * EDMA3 device handle created by EDMA3 initialization function.
 * The handle is initialized in function edma3init () implemented in file
 * bios_edma3_drv_sample_init.c
 */
extern EDMA3_DRV_Handle hEdma[];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 deinitSata(Void);
static Int32 initSata(Void);
static Void sataPscClkControl(Uint32 enable);

/* ========================================================================== */
/*                        EXTERN FUNCTION 			                           */
/* ========================================================================== */
extern Int32 edma3init(void);

/**
 *	\brief	enable or disable sata clock power down
 *
 *	\param	None
 *
 *  \return None
 */
Void sataClkPowerDownCtrl(Uint32 enable)
{
	if( enable )
	{
		*(volatile Uint32 *)SATACTRL_PWRDN = 0; 
	}
	else
	{
		*(volatile Uint32 *)SATACTRL_PWRDN = 1; 
	}
}

/**
 *	\brief	configure the sata phy control register 
 *
 *	\param	None
 *
 *	\return None
 */
void configureSataPhy (void)
{
	Uint32 phy_ctrl_val ;

	CSL_SataRegsOvly sataRegs = (CSL_SataRegsOvly)CSL_SATA_0_REGS;
	

	phy_ctrl_val = ( SATA_PHYCR_MPY_VAL | SATA_PHYCR_ULTR_BAND_WIDTH 
					 | SATA_PHYCR_STD_OSCSEL_EN | SATA_PHYCR_RXTERM_VDDA1
					 | SATA_PHYCR_RXCDR_6 | SATA_PHYCR_RXEQ | SATA_PHYCR_TXSWING 
					 | SATA_PHYCR_ENPLL );
				
	sataRegs->P0PHYCR = phy_ctrl_val; //0x80183958;

	sataClkPowerDownCtrl(1);
}

/**
 *	\brief	enable the SATA clock 
 *
 *	\param	None
 *
 *	\return None
 */
static Void sataPscClkControl(Uint32 enable)
{
	Uint32 sataDepCnt = 0xFFFF;
	if( enable )
	{
#ifdef BIOS_PWRM_ENABLE
		PWRM_setDependency(PWRM_RSRC_SATA);
#else
#ifdef BIOSATA_PSC_ENABLE
	    /* Bring the SATA module out of sleep state                               */
	    Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_SATA, TRUE); 
#endif
#endif
	}
	else
	{
#ifdef BIOS_PWRM_ENABLE
		do 
		{
			PWRM_releaseDependency(PWRM_RSRC_SATA);
			PWRM_getDependencyCount(PWRM_RSRC_SATA, &sataDepCnt);
		} while (sataDepCnt);
#else
#ifdef BIOSATA_PSC_ENABLE
	    /* Bring the SATA module out of sleep state                               */
	    Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_SATA, FALSE); 
#endif
#endif
	}
}

/**
 *  \brief Deinitialize SATA driver
 *
 *	\param	None
 *
 *  \return IOM_COMPLETED if success, error otherwise.
 */
static Int32 deinitSata(Void)
{
    Int32 result;

    result = PSP_sataDrvDeInit(SATA_INST_ID);

    return result;
}

/**
 *  \brief Initialize SATA driver.
 *
 *  This function will Initialize driver and wait for device to be registered
 *  if device is detected.
 *
 *	\param	edmaHandle	[IN]	edma handle
 *
 *  \return IOM_COMPLETED if success, error otherwise.
 */
static Int32 initSata(Void)
{
    Int32 result = IOM_COMPLETED;

#ifdef BIOS_PWRM_ENABLE
	/* initialize the PLL clock domain */
	SataRes[SATA_INST_ID].pll_domain = PWRM_CPU;
#endif
	result = PSP_sataDrvInit(SATA_INST_ID, SataRes);
	if (IOM_COMPLETED != result)
	{
            /* Driver init failed */
	}

    return result;
}

/**
 *	\brief	Platform init for the sata storage subsytem
 *			This function enable the sata clock and initializes
 *			sata phy and disables the sata clock. Because the
 *			phy initialization need to be done for sata port after sata 
 *			psc	clock is enabled. Further PowerManagment contorls (PWRM/LPSC)
 *			can enable/disble the clock without re-initialing the sata phy. 
 *
 *	\param	None
 *
 *  \return None
 */
Void sataPlatformInit(Void)
{
	/* enable the sata clock */
	sataPscClkControl(1) ;

	/*
	 * configure the sata phy control register 
	 */
	configureSataPhy();

	/* sata psc clock is disabled, the power management through
	   PWRM or LSPC control will enable/disable clock based on the need.
	   */
	sataPscClkControl(0) ;
}

/**
 *	\brief	Initialize the sata storage subsytem
 *
 *	\param	None
 *
 *  \return IOM_COMPLETED if success, error otherwise.
 */
Int32 sataStorageInit()
{
    Int32 result;

   /**
    * Initialize the EDMA
    */
    if (EDMA3_DRV_SOK != edma3init())
    {
        /* EDMA Initialization Failed */
    }

    result = PSP_blkmediaDrvInit(hEdma[0], 0x00, 0x02u, 4096u);
    if (IOM_COMPLETED == result)
    {
        result = initSata();
        if (IOM_COMPLETED != result)
        {
            /* Init SATA failed */
        }
    }
    else
    {
        /* BLKMED Init fails. */
    }
    return result;
}

/**
 *	\brief	DeInitialize the sata storage subsytem
 *
 *	\param	None
 *
 *  \return IOM_COMPLETED if success, error otherwise.
 */
Int32 sataStorageDeInit()
{
    Int32 result = IOM_COMPLETED;
    result = deinitSata();
    if (IOM_COMPLETED == result)
    {
        result = PSP_blkmediaDrvDeInit();
        if (IOM_COMPLETED != result)
        {
            /* BLKMED Deinit fails */
        }
    }
    else
    {
        /* SATA Driver de-init. failed */
    }

   // if (EDMA3_DRV_SOK != edma3deinit())
    {
        /* EDMA Driver deletion failed */
    }
    return result;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
