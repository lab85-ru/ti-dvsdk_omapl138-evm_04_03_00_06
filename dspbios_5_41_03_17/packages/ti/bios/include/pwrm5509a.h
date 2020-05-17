/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm5509a.h ========
 *
 *  DSP/BIOS Power Manager definitions for the C5509A device.
 *
 */

#ifndef PWRM5509A_
#define PWRM5509A_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWRM_5509A_DMA_DOMAIN = 0,
    PWRM_5509A_PERI_DOMAIN,
    PWRM_5509A_CLKOUT,
    PWRM_5509A_HPI,
    PWRM_5509A_TIMER0,
    PWRM_5509A_TIMER1,
    PWRM_5509A_MCBSP0,
    PWRM_5509A_MCBSP1,
    PWRM_5509A_MCBSP2,
    PWRM_5509A_I2C,
    PWRM_5509A_ADC,
    PWRM_5509A_USB,
    PWRM_5509A_MMCSD1,
    PWRM_5509A_MMCSD2,
    PWRM_5509A_RSRC_END
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

#endif /* PWRM5509A_ */
