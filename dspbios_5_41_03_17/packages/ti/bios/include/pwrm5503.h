/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm5503.h ========
 *
 *  DSP/BIOS Power Manager definitions for the C5503 device.
 *
 */

#ifndef PWRM5503_
#define PWRM5503_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWRM_5503_DMA_DOMAIN = 0,
    PWRM_5503_PERI_DOMAIN,
    PWRM_5503_CLKOUT,
    PWRM_5503_HPI,
    PWRM_5503_TIMER0,
    PWRM_5503_TIMER1,
    PWRM_5503_MCBSP0,
    PWRM_5503_MCBSP1,
    PWRM_5503_MCBSP2,
    PWRM_5503_I2C,
    PWRM_5503_RSRC_END
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

#endif /* PWRM5503_ */
