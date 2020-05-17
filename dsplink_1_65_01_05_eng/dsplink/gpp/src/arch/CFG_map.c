/** ============================================================================
 *  @file   CFG_map.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/
 *
 *  @desc   Defines the configuration mapping information for the DSP/BIOS LINK
 *          driver.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (POOL_COMPONENT)
#include <pooldefs.h>
#endif /* if defined (POOL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ------------------------------------- Generic Functions         */
#include <gen_utils.h>
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ------------------------------------- Link Driver               */
#include <cfgmap.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_CFG


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_CFG, __LINE__)
#else
#define SET_FAILURE_REASON   { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   CONFIG_maps
 *
 *  @desc   Array of configuration objects for the DSPs in the system.
 *  ============================================================================
 */
CONST CFGMAP_Object * CFGMAP_Config [MAX_DSPS] ;


/** ============================================================================
 *  @name   <device>MAP_Config
 *
 *  @desc   Forward declaration of DSP/BIOS LINK configuration mapping
 *          structure.
 *  ============================================================================
 */
#if defined (DM6446GEM)
extern CFGMAP_Object  DM6446GEMMAP_Config ;
#endif /* if defined (DM6446GEM) */

#if defined (DM6467GEM)
extern CFGMAP_Object  DM6467GEMMAP_Config ;
#endif/* if defined (DM6467GEM) */

#if defined (DM6437)
extern CFGMAP_Object  DM6437MAP_Config ;
#endif /* if defined (DM6437) */

#if defined (DM642)
extern CFGMAP_Object  DM648MAP_Config ;
#endif /* if defined (DM642) */

#if defined (DM648)
extern CFGMAP_Object  DM648MAP_Config ;
#endif /* if defined (DM648) */

#if defined (DRX416GEM)
extern CFGMAP_Object  DRX416GEMMAP_Config ;
#endif /* if defined (DRX416GEM) */

#if defined (DRA44XGEM)
extern CFGMAP_Object  DRA44XGEMMAP_Config ;
#endif /* if defined (DRA44XGEM) */

#if defined (OMAP2530)
extern CFGMAP_Object  OMAP2530MAP_Config ;
#endif /* if defined (OMAP2530) */

#if defined (OMAP3530)
extern CFGMAP_Object  OMAP3530MAP_Config ;
#endif /* if defined (OMAP3530) */

#if defined (DA8XXGEM)
extern CFGMAP_Object  DA8XXGEMMAP_Config ;
#endif /* if defined (DA8XXGEM) */

#if defined (TNETV107XGEM)
extern CFGMAP_Object  TNETV107XGEMMAP_Config ;
#endif /* if defined (DM6446GEM) */



/** ============================================================================
 *  @name   CFGMAP_objDB
 *
 *  @desc   Database containing the CFGMAP objects and assocaited DSP name.
 *  ============================================================================
 */
CONST CFGMAP_ObjDB CFGMAP_objDB [] = {
#if defined (DM6446GEM)
                                        {
                                            "DM6446GEM",
                                            &DM6446GEMMAP_Config,
                                        },
#endif  /* if defined (DM6446GEM) */
#if defined (DM6467GEM)
                                        {
                                            "DM6467GEM",
                                            &DM6467GEMMAP_Config,
                                        },
#endif  /* if defined (DM6467GEM) */
#if defined (DM6437)
                                        {
                                            "DM6437",
                                            &DM6437MAP_Config,
                                        },
#endif  /* if defined (DM6437) */
#if defined (DM648)
                                        {
                                            "DM648",
                                            &DM648MAP_Config,
                                        },
#endif  /* if defined (DM648) */
#if defined (DM642)
                                        {
                                            "DM642",
                                            &DM642MAP_Config,
                                        },
#endif  /* if defined (DM642) */
#if defined (DRX416GEM)
                                        {
                                            "DRX416GEM",
                                            &DRX416GEMMAP_Config,
                                        },
#endif  /* if defined (DRX416GEM) */
#if defined (DRA44XGEM)
                                        {
                                            "DRA44XGEM",
                                            &DRA44XGEMMAP_Config,
                                        },
#endif  /* if defined (DRA44XGEM) */
#if defined (OMAP2530)
                                        {
                                            "OMAP2530",
                                            &OMAP2530MAP_Config,
                                        },
#endif  /* if defined (OMAP2530) */
#if defined (OMAP3530)
                                        {
                                            "OMAP3530",
                                            &OMAP3530MAP_Config,
                                        },
#endif  /* if defined (OMAP3530) */
#if defined (DA8XXGEM)
                                        {
                                            "DA8XXGEM",
                                            &DA8XXGEMMAP_Config,
                                        },
#endif  /* if defined (DA8XXGEM) */
#if defined (TNETV107XGEM)
                                        {
                                            "TNETV107XGEM",
                                            &TNETV107XGEMMAP_Config,
                                        },
#endif  /* if defined (DM6446GEM) */

                               } ;

/** ============================================================================
 *  @func   CFGMAP_attachObject
 *
 *  @desc   Plugs the CFGMAP object at correct place in CFGMAP_Config array.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CFGMAP_attachObject (IN ProcessorId procId, IN Char8 * dspName)
{
    DSP_STATUS  status = DSP_SOK  ;
    Uint32      i      = 0        ;
    Int32       strCmpResult      ;

    TRC_2ENTER ("CFGMAP_attachObject", procId, dspName) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (NULL != dspName) ;

    if (IS_VALID_PROCID (procId) && (NULL != dspName)) {
        for (i = 0 ; i < MAX_DSPS ; i++) {
            status = GEN_Strcmp (dspName,
                                 CFGMAP_objDB [i].dspName,
                                 &strCmpResult) ;
                if (strCmpResult == 0) {
                    CFGMAP_Config [procId] = CFGMAP_objDB [i].obj ;
                    break ;
                }
        }
    }

    if (i == MAX_DSPS) {
	PRINT_Printf ("Configuration error:"
                      " Incorrect DSP name specified [%s]\n",
                      dspName) ;
        status = DSP_ECONFIG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("CFGMAP_attachObject", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


