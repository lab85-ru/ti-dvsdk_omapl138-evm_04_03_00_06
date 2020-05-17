/*   =========================================================
 *   DSP/BIOS LINK Configuration file.
 *
 *   CAUTION! This is a generated file.
 *            All changes will be lost.
 *
 *   This file was generated on MAR 30, 2017  20:12:16
 *   Target platform for DSP/BIOS LINK: OMAPL138
 *   =========================================================
 */

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <procdefs.h>
#include <_dsplink.h>

#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */

extern LINKCFG_Gpp LINKCFG_gppObject ;
extern LINKCFG_DspConfig DA8XXGEM_SHMEM_Config ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Configuration object for the overall system.
 *  ============================================================================
 */
LINKCFG_Object LINKCFG_config = { &LINKCFG_gppObject,
                                        {&DA8XXGEM_SHMEM_Config, } ,
                                      }  ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
