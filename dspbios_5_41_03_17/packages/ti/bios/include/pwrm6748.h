/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm6748.h ========
 *
 *  DSP/BIOS Power Manager definitions for the C6748 device.
 *
 */

#ifndef PWRM6748_
#define PWRM6748_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWRM_RSRC_EDMA3_CC_0   = 0,
    PWRM_RSRC_EDMA3_TC_0   = 1,
    PWRM_RSRC_EDMA3_TC_1   = 2,
    PWRM_RSRC_EMIFA        = 3,
    PWRM_RSRC_SPI_0        = 4,
    PWRM_RSRC_MMCSD_0      = 5,
    PWRM_RSRC_AINTC        = 6,
    PWRM_RSRC_RSVD1        = 7,
    PWRM_RSRC_UART_0       = 8,
    PWRM_RSRC_RSVD2        = 9,
    PWRM_RSRC_EDMA3_CC_1   = 10,
    PWRM_RSRC_USB0         = 11,
    PWRM_RSRC_USB1         = 12,
    PWRM_RSRC_GPIO         = 13,
    PWRM_RSRC_UHPI         = 14,
    PWRM_RSRC_EMAC         = 15,
    PWRM_RSRC_DDR2         = 16,
    PWRM_RSRC_MCASP_0      = 17,
    PWRM_RSRC_SATA         = 18,
    PWRM_RSRC_VPIF         = 19,
    PWRM_RSRC_SPI_1        = 20,
    PWRM_RSRC_I2C_1        = 21,
    PWRM_RSRC_UART_1       = 22,
    PWRM_RSRC_UART_2       = 23,
    PWRM_RSRC_MCBSP_0      = 24,
    PWRM_RSRC_MCBSP_1      = 25,
    PWRM_RSRC_LCDC         = 26,
    PWRM_RSRC_EHRPWM       = 27,
    PWRM_RSRC_MMCSD_1      = 28,
    PWRM_RSRC_UPP          = 29,
    PWRM_RSRC_ECAP         = 30,
    PWRM_RSRC_EDMA3_TC_2   = 31,
    PWRM_RSRC_END          = 32
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

#endif /* PWRM6748_ */
