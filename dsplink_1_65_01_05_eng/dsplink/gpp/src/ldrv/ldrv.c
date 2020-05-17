/** ============================================================================
 *  @file   ldrv.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Implementation of initialization and finalization functionality
 *          for LDRV.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <print.h>

/*  ----------------------------------- Link Driver                 */
#include <ldrv.h>
#include <ldrv_os.h>
#include <ldrv_proc.h>
#if !(defined (ONLY_PROC_COMPONENT))
#include <ldrv_smm.h>
#include <ldrv_drv.h>
#include <ldrv_ips.h>
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

#if defined (POOL_COMPONENT)
#include <ldrv_pool.h>
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
#include <ldrv_mpcs.h>
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
#include <ldrv_mplist.h>
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
#include <ldrv_mqt.h>
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
#include <ldrv_data.h>
#endif /* if defined (CHNL_COMPONENT) */

#if defined (RINGIO_COMPONENT)
#include <ldrv_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
LINKCFG_Object * LDRV_LinkCfgPtr = NULL ;


/** ============================================================================
 *  @func   LDRV_moduleInit
 *
 *  @desc   This function initializes the LDRV module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_moduleInit (IN LINKCFG_Object * linkCfg)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_1ENTER ("LDRV_moduleInit", linkCfg) ;

    DBC_Require (linkCfg != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Get the pointer to kernel-side DSPLINK configuration structure.
     *  ------------------------------------------------------------------------
     */
    status = LDRV_getLinkGppCfg (linkCfg, &LDRV_LinkCfgPtr) ;

    /*  ------------------------------------------------------------------------
     *  Call the module initialization functions for all modules.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        status = LDRV_PROC_moduleInit () ;
#if (defined (ONLY_PROC_COMPONENT))
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
#if !(defined (ONLY_PROC_COMPONENT))
        if (DSP_SUCCEEDED (status)) {
            status = LDRV_SMM_moduleInit () ;
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_DRV_moduleInit () ;
                if (DSP_SUCCEEDED (status)) {
                    status = LDRV_IPS_moduleInit () ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

#if defined (POOL_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_POOL_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_MPCS_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_MPLIST_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_MQT_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_DATA_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (RINGIO_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_RINGIO_moduleInit () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (RINGIO_COMPONENT) */
        }
        else {
            SET_FAILURE_REASON ;
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
    }

    TRC_1LEAVE ("LDRV_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_moduleExit
 *
 *  @desc   This function finalizes the LDRV module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32      i        = 0       ;

    TRC_0ENTER ("LDRV_moduleExit") ;

    DBC_Require (LDRV_LinkCfgPtr != NULL) ;

    if (LDRV_LinkCfgPtr != NULL) {
#if !(defined (ONLY_PROC_COMPONENT))
        /*  --------------------------------------------------------------------
         *  Call the module finalization functions for all modules.
         *  --------------------------------------------------------------------
         */
#if defined (RINGIO_COMPONENT)
        status = LDRV_RINGIO_moduleExit () ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (CHNL_COMPONENT)
        tmpStatus = LDRV_DATA_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
        tmpStatus = LDRV_MQT_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (MPLIST_COMPONENT)
        tmpStatus = LDRV_MPLIST_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MPCS_COMPONENT)
        tmpStatus = LDRV_MPCS_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (POOL_COMPONENT)
        tmpStatus = LDRV_POOL_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (POOL_COMPONENT) */

        tmpStatus = LDRV_IPS_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = LDRV_DRV_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = LDRV_SMM_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        tmpStatus = LDRV_PROC_moduleExit () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        for ( i =0; i < MAX_DSPS ; i++ ) {
            /* Freeing the DSP configuration information
             * for all the confgured DSPs
             */
        	LDRV_freeLinkDspCfg (i, LDRV_LinkCfgPtr) ;
    	}
        /*  --------------------------------------------------------------------
         *  Free any memory allocated for the kernel-side DSPLINK configuration
         *  structure.
         *  --------------------------------------------------------------------
         */
        tmpStatus = LDRV_freeLinkGppCfg (LDRV_LinkCfgPtr) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
        LDRV_LinkCfgPtr = NULL ;
    }

    TRC_1LEAVE ("LDRV_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_init
 *
 *  @desc   Allocates resources and initializes the LDRV component for a DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_init (IN ProcessorId procId, IN LINKCFG_DspConfig * dspCfg)
{
    DSP_STATUS  status  = DSP_SOK ;
    Char8 *     dspName = NULL    ;

    TRC_2ENTER ("LDRV_init", procId, dspCfg) ;

    DBC_Require (LDRV_LinkCfgPtr != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    /*  ------------------------------------------------------------------------
     *  Get the pointer to kernel-side DSPLINK configuration structure.
     *  ------------------------------------------------------------------------
     */
    if (dspCfg != NULL) {
        if (LDRV_LinkCfgPtr->dspConfigs [procId] != NULL) {
            /* First free the previously read config values */
            status = LDRV_freeLinkDspCfg (procId, LDRV_LinkCfgPtr) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            status = LDRV_getLinkDspCfg (procId, dspCfg, LDRV_LinkCfgPtr) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                LDRV_exit (procId) ;
            }
        }
    }
    else {
        if (LDRV_LinkCfgPtr->dspConfigs [procId] == NULL) {
            PRINT_Printf ("For multi-app support, Please pass valid DSP") ;
            PRINT_Printf ("Config values through PROC_attach.\n") ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Plug the correct configuration mapping information for DSP/BIOS LINK.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        dspName = LDRV_LinkCfgPtr->dspConfigs [procId]->dspObject->name ;
        status = CFGMAP_attachObject (procId, dspName) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            LDRV_exit (procId) ;
        }
    }

    TRC_1LEAVE ("LDRV_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_exit
 *
 *  @desc   Releases resources used by this component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_exit (IN ProcessorId procId)
{
    DSP_STATUS status    = DSP_SOK ;

    TRC_1ENTER ("LDRV_exit", procId) ;

    TRC_1LEAVE ("LDRV_exit", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
