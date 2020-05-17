/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm3430.h ========
 *
 *  DSP/BIOS Power Manager definitions for the OMAP3430 device.
 *
 */

#ifndef PWRM3430_
#define PWRM3430_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWRM_3430_BIOS_CLK = 0,
    PWRM_3430_GPTIMER_5,
    PWRM_3430_GPTIMER_6,
    PWRM_3430_GPTIMER_7,
    PWRM_3430_GPTIMER_8,
    PWRM_3430_MCBSP_1,
    PWRM_3430_MCBSP_2,
    PWRM_3430_MCBSP_3,
    PWRM_3430_MCBSP_4,
    PWRM_3430_MCBSP_5,
    PWRM_3430_SSI,
    PWRM_3430_LEON,
    PWRM_3430_USER_BASE
} PWRM_Resource;


/*
 *  ======== PWRM_getDependencyCount ========
 *  Get the count of dependencies that are currently declared on a resource.
 *
 */
extern PWRM_Status PWRM_getDependencyCount(PWRM_Resource resourceID,
    Uns * count);


/*
 *  ======== PWRM_releaseDependency ========
 *  Release a dependency that has been previously declared by 
 *  PWRM_setDependency.
 *
 */
extern PWRM_Status PWRM_releaseDependency(PWRM_Resource resourceID);


/*
 *  ======== PWRM_setDependency ========
 *  Declare a dependency upon a power-manageable resource.
 *
 */
extern PWRM_Status PWRM_setDependency(PWRM_Resource resourceID);


#ifdef __cplusplus
}
#endif

#endif /* PWRM3430_ */
