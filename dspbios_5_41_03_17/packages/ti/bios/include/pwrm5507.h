/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm5507.h ========
 *
 *  DSP/BIOS Power Manager definitions for the C5507 device.
 *
 */

#ifndef PWRM5507_
#define PWRM5507_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWRM_5507_DMA_DOMAIN = 0,
    PWRM_5507_PERI_DOMAIN,
    PWRM_5507_CLKOUT,
    PWRM_5507_HPI,
    PWRM_5507_TIMER0,
    PWRM_5507_TIMER1,
    PWRM_5507_MCBSP0,
    PWRM_5507_MCBSP1,
    PWRM_5507_MCBSP2,
    PWRM_5507_I2C,
    PWRM_5507_ADC,
    PWRM_5507_USB,
    PWRM_5507_RSRC_END
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

#endif /* PWRM5507_ */
