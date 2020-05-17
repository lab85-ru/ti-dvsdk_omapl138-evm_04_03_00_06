/** ============================================================================
 *  @file   ldrv_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/Linux/
 *
 *  @desc   Implementation of OS-specific functionality for LDRV.
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
#include <mem.h>
#include <user.h>

/*  ----------------------------------- Link Driver                 */
#include <ldrv_os.h>


#if defined (__cplusplus)
extern "C" {
#endif

/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_OS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_OS, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   LDRV_getLinkGppCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies only GPP structures.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkGppCfg (IN  LINKCFG_Object *  linkCfg,
                    OUT LINKCFG_Object ** knlLinkCfg)
{
    DSP_STATUS       status = DSP_SOK ;
    LINKCFG_Object * knlCfg = NULL ;
    Void *           tmpUsrPtr ;
    Uint32           size ;
    Uint32           procId ;
    Uint32           i      ;

    TRC_2ENTER ("LDRV_getLinkCfg", linkCfg, knlLinkCfg) ;

    DBC_Require (linkCfg    != NULL) ;
    DBC_Require (knlLinkCfg != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Allocate memory for the global LINKCFG object and initialize it.
     *  ------------------------------------------------------------------------
     */
    status = MEM_Calloc ((Void **) knlLinkCfg,
                         sizeof (LINKCFG_Object),
                         MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        knlCfg = *knlLinkCfg ;

        /* Copy the contents of the object from user-space. Return value
         * indicates number of bytes that were not copied.
         */
        status = USER_copyFromUser ((Void *) knlCfg,
                                    (Void *) linkCfg,
                                    sizeof (LINKCFG_Object)) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the GPP object and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlCfg->gppObject ;
        size      = sizeof (LINKCFG_Gpp) ;
        /* Allocate memory for the GPP object. */
        status = MEM_Alloc ((Void **) &(knlCfg->gppObject),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the object from user-space. */
            status = USER_copyFromUser ((Void *) (knlCfg->gppObject),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Get the DSP Config if available, this ensures that multi-process works,
     *  If values provided at setup are from a process, proc_attach done by some
     *  other process then copy_to_user must pass.
     *  ------------------------------------------------------------------------
     */
    for (procId  = 0 ;
         (procId < MAX_DSPS) && (DSP_SUCCEEDED (status)) ;
         ) {
        status = LDRV_getLinkDspCfg (procId,
                                     knlCfg->dspConfigs [procId],
                                     *knlLinkCfg) ;
        if (DSP_SUCCEEDED(status))
        procId++ ;
    }

    if (DSP_FAILED (status)) {
        /* Free the copied DSP values */
        for (i = 0 ; i < procId ; i++) {
            LDRV_freeLinkDspCfg (i, *knlLinkCfg) ;
        }

        /* Free the GPP value */
        LDRV_freeLinkGppCfg (knlCfg) ;
    }

    TRC_1LEAVE ("LDRV_getLinkGppCfg", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_getLinkDspCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies only DSP structures.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkDspCfg (IN  ProcessorId         procId,
                    IN  LINKCFG_DspConfig * dspCfg,
                    OUT LINKCFG_Object *    knlLinkCfg)
{
    DSP_STATUS          status        = DSP_SOK ;
    LINKCFG_DspConfig * knlDspCfg     = NULL ;
    LINKCFG_Dsp *       dspObject     = NULL ;
#if !(defined (ONLY_PROC_COMPONENT))
    LINKCFG_LinkDrv *   linkDrvObject = NULL ;
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
    Void *              tmpUsrPtr ;
    Void *              tmpKnlPtr ;
    Uint32              size ;
    Uint32              tableId ;

    TRC_3ENTER ("LDRV_getLinkDspCfg",procId, dspCfg, knlLinkCfg) ;

    DBC_Require (knlLinkCfg != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (DSP_SUCCEEDED (status)) {
        size = sizeof (LINKCFG_DspConfig) ;
        /* Allocate memory for the DSP object. */
        status = MEM_Alloc ((Void **) &(tmpKnlPtr),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *) tmpKnlPtr,
                                        (void *) dspCfg,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                knlLinkCfg->dspConfigs [procId] = tmpKnlPtr ;
                knlDspCfg = tmpKnlPtr ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the DSP and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = knlDspCfg->dspObject ;
        size      = sizeof (LINKCFG_Dsp) ;
        /* Allocate memory for the DSP object. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->dspObject),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *) knlDspCfg->dspObject,
                                        (void *) tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                dspObject = knlDspCfg->dspObject ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the Link driver object array and initialize
     *  it.
     *  ------------------------------------------------------------------------
     */
#if !(defined (ONLY_PROC_COMPONENT))
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->linkDrvObjects ;
        size      = sizeof (LINKCFG_LinkDrv) *  knlDspCfg->numDrvs ;
        /* Allocate memory for the Link Driver object array. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->linkDrvObjects),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->linkDrvObjects),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                linkDrvObject =
                             &knlDspCfg->linkDrvObjects [dspObject->linkDrvId] ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the array of memory tables and initialize
     *  it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->memTables ;
        size = sizeof (LINKCFG_MemEntry *) * knlDspCfg->numMemTables ;
        /* Allocate memory for the array of pointers to memory tables. */
        status = MEM_Calloc ((Void **) &(knlDspCfg->memTables),
                             size,
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the array of pointers from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->memTables),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                tableId   = knlDspCfg->dspObject->memTableId ;
                tmpUsrPtr = (Void *) knlDspCfg->memTables [tableId] ;
                size =  sizeof (LINKCFG_MemEntry)
                      * knlDspCfg->dspObject->memEntries ;

                /* Allocate memory for the memory table. */
                status = MEM_Alloc ((Void **)
                                    &(knlDspCfg->memTables [tableId]),
                                    size,
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Copy the contents of the objects from user-space. */
                    status = USER_copyFromUser (
                                      (Void *) (knlDspCfg->memTables [tableId]),
                                                tmpUsrPtr,
                                                size) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

#if !(defined (ONLY_PROC_COMPONENT))
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the array of IPS tables and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->ipsTables ;
        size      = sizeof (LINKCFG_Ips *) * knlDspCfg->numIpsTables ;
        /* Allocate memory for the array of pointers to IPS tables. */
        status = MEM_Calloc ((Void **) &(knlDspCfg->ipsTables),
                             size,
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the array of pointers from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->ipsTables),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                tableId   = linkDrvObject->ipsTableId;
                tmpUsrPtr = (Void *) knlDspCfg->ipsTables [tableId] ;
                size =   sizeof (LINKCFG_Ips) * linkDrvObject->numIpsEntries ;

                /* Allocate memory for the IPS table. */
                status = MEM_Alloc ((Void **)
                                    &(knlDspCfg->ipsTables [tableId]),
                                    size,
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Copy the contents of the objects from user-space. */
                    status = USER_copyFromUser ((Void *)
                                      (knlDspCfg->ipsTables [tableId]),
                                      tmpUsrPtr,
                                      size) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

#if defined (POOL_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the array of Pool tables and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->poolTables ;
        size      = sizeof (LINKCFG_Pool *) * knlDspCfg->numPoolTables ;
        /* Allocate memory for the array of pointers to Pool tables. */
        status = MEM_Calloc ((Void **) &(knlDspCfg->poolTables),
                             size,
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the array of pointers from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->poolTables),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                tableId = linkDrvObject->poolTableId ;
                tmpUsrPtr = (Void *) knlDspCfg->poolTables [tableId] ;
                size = sizeof (LINKCFG_Pool) * linkDrvObject->numPools ;

                /* Allocate memory for the Pool table. */
                status = MEM_Alloc ((Void **)
                                   &(knlDspCfg->poolTables [tableId]),
                                   size,
                                   MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Copy the contents of the objects from user-space. */
                    status = USER_copyFromUser (
                                     (Void *) (knlDspCfg->poolTables [tableId]),
                                     tmpUsrPtr,
                                     size) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (POOL_COMPONENT) */

#if defined (CHNL_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the array of Data driver tables and
     *  initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->dataTables ;
        size = sizeof (LINKCFG_DataDrv *) * knlDspCfg->numDataTables ;
        /* Allocate memory for the array of pointers to Data driver tables. */
        status = MEM_Calloc ((Void **) &(knlDspCfg->dataTables),
                             size,
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the array of pointers from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->dataTables),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                tableId = linkDrvObject->dataTableId ;
                tmpUsrPtr = (Void *) knlDspCfg->dataTables [tableId] ;
                size =  sizeof (LINKCFG_DataDrv)
                      * linkDrvObject->numDataDrivers ;

                /* Allocate memory for the Data driver table. */
                status = MEM_Alloc ((Void **)
                                   &(knlDspCfg->dataTables [tableId]),
                                   size,
                                   MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Copy the contents of the objects from user-space. */
                    status = USER_copyFromUser (
                                     (Void *) (knlDspCfg->dataTables [tableId]),
                                     tmpUsrPtr,
                                     size) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the MQT object array and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->mqtObjects ;
        size      = sizeof (LINKCFG_Mqt) * knlDspCfg->numMqts ;
        /* Allocate memory for the MQT object array. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->mqtObjects),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->mqtObjects),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (RINGIO_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the RingIO object array and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->ringIoObjects ;
        size      = sizeof (LINKCFG_RingIo) * knlDspCfg->numRingIo ;
        /* Allocate memory for the RingIO object array. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->ringIoObjects),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *)(knlDspCfg->ringIoObjects),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MPLIST_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the MPLIST object array and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->mplistObjects ;
        size      = sizeof (LINKCFG_MpList) * knlDspCfg->numMpList ;
        /* Allocate memory for the MPLIST object array. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->mplistObjects),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *)(knlDspCfg->mplistObjects),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MPCS_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the MPCS object array and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->mpcsObjects ;
        size      = sizeof (LINKCFG_Mpcs) * knlDspCfg->numMpcs ;
        /* Allocate memory for the MPCS object array. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->mpcsObjects),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the objects from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->mpcsObjects),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (LOG_COMPONENT)
    /*  ------------------------------------------------------------------------
     *  Allocate memory required for the LOG object and initialize it.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        tmpUsrPtr = (Void *) knlDspCfg->logObject ;
        size      = sizeof (LINKCFG_Log) ;
        /* Allocate memory for the GPP object. */
        status = MEM_Alloc ((Void **) &(knlDspCfg->logObject),
                            size,
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Copy the contents of the object from user-space. */
            status = USER_copyFromUser ((Void *) (knlDspCfg->logObject),
                                        tmpUsrPtr,
                                        size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
#endif /* if defined (LOG_COMPONENT) */
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

    if (DSP_FAILED (status)) {
        LDRV_freeLinkDspCfg (procId, knlLinkCfg) ;
    }

    TRC_1LEAVE ("LDRV_getLinkDspCfg", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_freeLinkCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_freeLinkGppCfg (IN  LINKCFG_Object *  knlLinkCfg)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    TRC_1ENTER ("LDRV_freeLinkGppCfg", knlLinkCfg) ;

    if (knlLinkCfg != NULL) {
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the GPP object
         *  --------------------------------------------------------------------
         */
        if (knlLinkCfg->gppObject != NULL) {
            tmpStatus = FREE_PTR (knlLinkCfg->gppObject) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the global LINKCFG object.
         *  --------------------------------------------------------------------
         */
        tmpStatus = FREE_PTR (knlLinkCfg) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LDRV_freeLinkGppCfg", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_freeLinkCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_freeLinkDspCfg (IN  ProcessorId       procId,
                     IN  LINKCFG_Object *  knlLinkCfg)
{
    DSP_STATUS          status    = DSP_SOK ;
    DSP_STATUS          tmpStatus = DSP_SOK ;
    LINKCFG_DspConfig * dspCfg    = NULL    ;
    LINKCFG_Dsp *       dspObject     = NULL ;
#if !(defined (ONLY_PROC_COMPONENT))
    LINKCFG_LinkDrv *   linkDrvObject = NULL ;
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
    Uint32              tableId ;

    TRC_2ENTER ("LDRV_freeLinkDspCfg", procId, knlLinkCfg) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (knlLinkCfg != NULL) ;

    if (knlLinkCfg != NULL) {
        dspCfg        = knlLinkCfg->dspConfigs [procId] ;
        if (dspCfg != NULL && ((Pvoid) dspCfg >= KMEM_STARTADDRESS)) {
            dspObject     = dspCfg->dspObject ;
            if (dspObject != NULL && ((Pvoid) dspObject >= KMEM_STARTADDRESS)) {
#if !(defined (ONLY_PROC_COMPONENT))
                linkDrvObject = &dspCfg->linkDrvObjects [dspObject->linkDrvId] ;
                if (   (linkDrvObject != NULL)
                    && ((Pvoid) linkDrvObject >= KMEM_STARTADDRESS)) {
                }
                else {
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
            }
            else {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
#if defined (LOG_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the LOG object.
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->logObject != NULL)
            && (dspCfg->logObject >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->logObject) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (LOG_COMPONENT) */

#if !(defined (ONLY_PROC_COMPONENT))
#if defined (MPCS_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the MPCS objects
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->mpcsObjects != NULL)
            && ((Pvoid) dspCfg->mpcsObjects >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->mpcsObjects) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the MPLIST objects
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->mplistObjects != NULL)
            && ((Pvoid) dspCfg->mplistObjects >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->mplistObjects) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (RINGIO_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the RingIO objects
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->ringIoObjects != NULL)
            && ((Pvoid) dspCfg->ringIoObjects >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->ringIoObjects) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MSGQ_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the MQT objects
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->mqtObjects != NULL)
            && ((Pvoid) dspCfg->mqtObjects >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->mqtObjects) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the Data driver tables
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->dataTables != NULL)
            && ((Pvoid) dspCfg->dataTables >= KMEM_STARTADDRESS)) {
            tableId   = linkDrvObject->dataTableId ;
            tmpStatus = FREE_PTR (dspCfg->dataTables [tableId]) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = FREE_PTR (dspCfg->dataTables) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (POOL_COMPONENT)
        /*  --------------------------------------------------------------------
         *  Free memory allocated for the Pool tables
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->poolTables != NULL)
            && ((Pvoid) dspCfg->poolTables >= KMEM_STARTADDRESS)) {
            tableId   = linkDrvObject->poolTableId ;
            tmpStatus = FREE_PTR (dspCfg->poolTables [tableId]) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = FREE_PTR (dspCfg->poolTables) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (POOL_COMPONENT) */

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the IPS tables
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->ipsTables != NULL)
            && ((Pvoid) dspCfg->ipsTables >= KMEM_STARTADDRESS)) {
            tableId   = linkDrvObject->ipsTableId ;
            tmpStatus = FREE_PTR (dspCfg->ipsTables [tableId]) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = FREE_PTR (dspCfg->ipsTables) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the Memory tables
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->memTables != NULL)
            && ((Pvoid) dspCfg->memTables >= KMEM_STARTADDRESS)) {
            tableId   = dspCfg->dspObject->memTableId ;
            tmpStatus = FREE_PTR (dspCfg->memTables [tableId]) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = FREE_PTR (dspCfg->memTables) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the Link Driver objects
         *  --------------------------------------------------------------------
         */
#if !(defined (ONLY_PROC_COMPONENT))
        if (   (dspCfg->linkDrvObjects != NULL)
            && ((Pvoid) dspCfg->linkDrvObjects >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->linkDrvObjects) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the DSP objects
         *  --------------------------------------------------------------------
         */
        if (   (dspCfg->dspObject != NULL)
            && ((Pvoid) dspCfg->dspObject >= KMEM_STARTADDRESS)) {
            tmpStatus = FREE_PTR (dspCfg->dspObject) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the global DSP Config object.
         *  --------------------------------------------------------------------
         */
        tmpStatus = FREE_PTR (dspCfg) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        /* Delete the dspConfig anyway */
        knlLinkCfg->dspConfigs [procId] = NULL ;
    }

    TRC_1LEAVE ("LDRV_freeLinkDspCfg", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
