/** ============================================================================
 *  @file   ldrv_chirps.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Functions to manage list of LDRV_CHNL_Irp objects.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv.h>
#include <ldrv_chnl.h>
#include <ldrv_data.h>
#include <ldrv_chirps.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component ID of this module.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_LDRV_CHIRPS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_CHIRPS, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   LDRV_CHIRPS_CreateList
 *
 *  @desc   Allocates a chirp list.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
List *
LDRV_CHIRPS_CreateList (IN Uint32 numIRPs)
{
    DSP_STATUS       status    = DSP_SOK ;
    List *           chirpList = NULL    ;
    LDRV_CHNL_Irp *  chirp     = NULL    ;
    Uint32           i                   ;

    TRC_1ENTER ("LDRV_CHIRPS_CreateList", numIRPs) ;

    status = LIST_Create (&chirpList) ;
    if (DSP_SUCCEEDED (status)) {

        /* Make N chirps and place on queue. */
        for (i = 0 ; (i < numIRPs) && (DSP_SUCCEEDED (status)) ; i++) {
            chirp = LDRV_CHIRPS_Alloc () ;
            if (chirp != NULL) {
                status = LIST_PutTail (chirpList, (ListElement *) chirp) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                    FREE_PTR (chirp) ;
                }
            }
            else {
                status = DSP_EMEMORY ;
                SET_FAILURE_REASON ;
            }
        }

        /* If we couldn't allocate all chirps, free those allocated: */
        if ((i != numIRPs) || DSP_FAILED (status)) {
            LDRV_CHIRPS_FreeList (chirpList) ;  /* No care for status */
            chirpList = NULL ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHIRPS_CreateList", chirpList) ;

    return chirpList ;
}


/** ============================================================================
 *  @func   LDRV_CHIRPS_FreeList
 *
 *  @desc   deallocates a chirp list.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHIRPS_FreeList (IN List * chirpList)
{
    DSP_STATUS       status     = DSP_SOK ;
    DSP_STATUS       freeStatus = DSP_SOK ;
    LDRV_CHNL_Irp *  listHead             ;

    TRC_1ENTER ("LDRV_CHIRPS_FreeList", chirpList) ;

    if (chirpList != NULL) {
        while (!LIST_IsEmpty (chirpList)) {
            status = LIST_GetHead (chirpList,
                                   (ListElement **) &listHead) ;

            if (DSP_FAILED (status)) {
                freeStatus = status ;
                SET_FAILURE_REASON ;
            }
            else {
                status = FREE_PTR (listHead) ;
                if (DSP_FAILED (status)) {
                    freeStatus = status ;
                    SET_FAILURE_REASON ;
                }
            }
        }

        status = LIST_Delete (chirpList) ;
        if (DSP_SUCCEEDED (status)) {
            status = freeStatus ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LDRV_CHIRPS_FreeList", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHIRPS_Alloc
 *
 *  @desc   Allocates a single chirp.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHIRPS_Alloc (Void)
{
    DSP_STATUS       status = DSP_SOK ;
    LDRV_CHNL_Irp *  chirp  = NULL    ;

    TRC_0ENTER ("LDRV_CHIRPS_Alloc") ;

    status = MEM_Calloc ((Void **) &chirp, sizeof(LDRV_CHNL_Irp), MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        status = LIST_InitializeElement ((ListElement *) &(chirp->link)) ;
        if (DSP_FAILED (status)) {
            FREE_PTR (chirp) ;
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHIRPS_Alloc", chirp) ;

    return chirp ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_CHIRPS_Print
 *
 *  @desc   Prints a single CHIRP (Channel IO Request Packet).
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHIRPS_Print (IN LDRV_CHNL_Irp * chirp)
{
    if (chirp == NULL) {
        TRC_0PRINT (TRC_LEVEL4, "\nChirp is NULL\n") ;
    }
    else {
        TRC_5PRINT (TRC_LEVEL4,
                    "CHIRP [0x%x]:\n"
                    "    buffer    = 0x%x\n"
                    "    arg       = 0x%x\n"
                    "    size      = 0x%x\n"
                    "    iocStatus = 0x%x\n",
                    chirp, chirp->buffer, chirp->arg,
                    chirp->size, chirp->iocStatus) ;
    }
}


/** ============================================================================
 *  @func   LDRV_CHIRPS_Debug
 *
 *  @desc   Prints a CHIRP (Channel IO Request Packet) List.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHIRPS_Debug (IN List * chirpList)
{
    DSP_STATUS       status     = DSP_SOK ;
    LDRV_CHNL_Irp *  curElem    = NULL    ;
    LDRV_CHNL_Irp *  nextElem   = NULL    ;

    if (chirpList == NULL) {
        TRC_0PRINT (TRC_LEVEL4, "  List is NULL\n") ;
    }
    else if (LIST_IsEmpty (chirpList)) {
        TRC_0PRINT (TRC_LEVEL4, "  List is Empty\n") ;
    }
    else {
        TRC_0PRINT (TRC_LEVEL4, "  List is Non-Empty\n") ;
        TRC_0PRINT (TRC_LEVEL4, ">>>>ChirpList Elements: Start<<<<<\n") ;
        LIST_First (chirpList, (ListElement **) &curElem) ;
        LDRV_CHIRPS_Print (curElem) ;
        status = LIST_Next (chirpList, (ListElement *) curElem,
                            (ListElement **) &nextElem) ;
        while (DSP_SUCCEEDED (status)  && (nextElem != NULL)) {
            LDRV_CHIRPS_Print (nextElem) ;
            curElem = nextElem ;
            status = LIST_Next (chirpList, (ListElement *) curElem,
                                (ListElement **) &nextElem) ;
        }
        TRC_0PRINT (TRC_LEVEL4, ">>>>ChirpList Elements: End<<<<<\n") ;
    }
}
#endif  /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
