/** ============================================================================
 *  @file   ldrv_mplist.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MPLIST/
 *
 *  @desc   Declarations for the MPLIST for GPP-DSP communication.
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
#include <procdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>
#include <isr.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- User API                      */
#include <mplist.h>

/*  ----------------------------------- Link Driver                   */
#include <dsp.h>
#include <ldrv_pool.h>
#include <ldrv_mplist.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_MPLIST


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_MPLIST, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPLIST_Object
 *
 *  @desc   This structure defines the LDRV MPLIST object, which contains all
 *          state information required by the LDRV MPLIST component.
 *
 *  @field  ctrlPtr
 *              Pointer to the MPLIST shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the MPLIST shared memory control region in DSP
 *              address space.
 *  @field  physCtrlAddr
 *              Address of the MPLIST shared memory control region in physical
 *              address space.
 *  @field  ctrlSize
 *              Size of the MPLIST shared memory control region.
 *  @field  memEntry
 *              Entry ID of the MPLIST shared memory control region.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_MPLIST_Object_tag {
    MPLIST_Ctrl *   ctrlPtr ;
    Uint32          dspCtrlAddr ;
    Uint32          physCtrlAddr ;
    Uint32          ctrlSize ;
    Uint32          memEntry ;
    Bool            wordSwap ;
} LDRV_MPLIST_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPLIST_State
 *
 *  @desc   State object for the LDRV_MPLIST component containing all
 *          information required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_MPLIST_Object LDRV_MPLIST_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPLIST_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_MPLIST_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_MPLIST_moduleInit
 *
 *  @desc   This function initializes the LDRV_MPLIST module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_moduleInit (Void)
{
    DSP_STATUS           status = DSP_SOK ;
    Uint32               dspId ;

    TRC_0ENTER ("LDRV_MPLIST_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_MPLIST_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_MPLIST_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPLIST_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MPLIST module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_MPLIST_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_MPLIST_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_MPLIST_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MPLIST_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPLIST_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS           status = DSP_SOK ;
    LINKCFG_MpList *     mplistObject ;
    LINKCFG_Dsp *        dspObj ;
    LINKCFG_DspConfig *  dspCfg ;
    LINKCFG_LinkDrv *    linkDrv ;
    LDRV_MPLIST_Object * mplistState ;

    TRC_2ENTER ("LDRV_MPLIST_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    mplistState  = &(LDRV_MPLIST_State [dspId]) ;

    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    mplistObject = &(dspCfg->mplistObjects [linkDrv->mplistTableId]) ;

    *shDspAddr = (Uint32) NULL ;
    if (LDRV_MPLIST_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the link driver.
         *  --------------------------------------------------------------------
         */
        mplistState = &(LDRV_MPLIST_State [dspId]) ;

        /* Initialize the state object. */
        mplistState->ctrlPtr      = NULL ;
        mplistState->dspCtrlAddr  = (Uint32) NULL ;
        mplistState->physCtrlAddr = (Uint32) NULL ;
        mplistState->ctrlSize     = 0 ;
        mplistState->memEntry     = mplistObject->memEntry ;
        mplistState->wordSwap     = dspObj->wordSwap ;

        /* Check if the memEntry is in valid range. */
        if (    (mplistObject->memEntry != (Uint32) -1)
            &&  (mplistObject->memEntry >= dspObj->memEntries)) {
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPLIST memEntry specified [0x%x]\n",
                          mplistObject->memEntry) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (mplistObject->ipsId != (Uint32) -1)
                 &&  (mplistObject->ipsId >= linkDrv->numIpsEntries)) {
            /* Check if the ipsId is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPLIST ipsId specified [0x%x]\n",
                          mplistObject->ipsId) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (mplistObject->ipsId      != (Uint32) -1)
                 &&  (mplistObject->ipsEventNo != (Uint32) -1)
                 &&  (   mplistObject->ipsEventNo
                      >= dspCfg->ipsTables [linkDrv->ipsTableId]
                                          [mplistObject->ipsId].numIpsEvents)) {
            /* Check if the ipsEventNo is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPLIST ipsEventNo specified [0x%x]\n",
                          mplistObject->ipsEventNo) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Added for MISRAC compliance */
        }
    }

    if (   (LDRV_MPLIST_IsInitialized [dspId] == FALSE)
        && (DSP_SUCCEEDED (status))) {
        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the MPLIST component.
         *  --------------------------------------------------------------------
         */
        mplistState->ctrlSize = sizeof (MPLIST_Ctrl)
                         +  (sizeof (MPLIST_Entry) * mplistObject->maxEntries) ;

        status = LDRV_SMM_alloc (dspId,
                                 mplistState->memEntry,
                                 &(mplistState->physCtrlAddr),
                                 &(mplistState->dspCtrlAddr),
                                 (Uint32 *) ((Pvoid) &(mplistState->ctrlPtr)),
                                 mplistState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = mplistState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPLIST Ctrl GPP address      [0x%x]\n",
                        mplistState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPLIST Ctrl DSP address      [0x%x]\n",
                        mplistState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPLIST Ctrl physical address [0x%x]\n",
                        mplistState->physCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPLIST Ctrl size             [0x%x]\n",
                        mplistState->ctrlSize) ;

            /* mplistState->ctrlPtr->entry is set on user-side with user
             * address.
             */
            mplistState->ctrlPtr->dspAddrEntry = (MPLIST_Entry *)
                                         (   (Uint32) (mplistState->dspCtrlAddr)
                                          +  sizeof (MPLIST_Ctrl)) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        LDRV_MPLIST_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_MPLIST_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_MPLIST_exit (dspId) ;
            LDRV_MPLIST_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_MPLIST_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPLIST_exit
 *
 *  @desc   This function finalizes the LDRV MPLIST module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_exit (IN ProcessorId dspId)
{
    DSP_STATUS           status    = DSP_SOK ;
    LDRV_MPLIST_Object * mplistState ;

    TRC_1ENTER ("LDRV_MPLIST_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_MPLIST_IsInitialized [dspId] == TRUE) {
        mplistState = &(LDRV_MPLIST_State [dspId]) ;

        /*  --------------------------------------------------------------------
         *  Free shared memory allocated for the MPLIST component.
         *  --------------------------------------------------------------------
         */
        if (mplistState->ctrlPtr != NULL) {
            status = LDRV_SMM_free (dspId,
                                    mplistState->memEntry,
                                    mplistState->ctrlSize) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        LDRV_MPLIST_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_MPLIST_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPLIST_isEmpty
 *
 *  @desc   check for an empty list.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Bool
LDRV_MPLIST_isEmpty (IN     ProcessorId dspId,
                     IN     List *      list )
{
    Bool                 retVal = FALSE ;
    LDRV_MPLIST_Object * mplistState ;
    Uint32               temp   ;

    TRC_2ENTER ("LDRV_MPLIST_isEmpty", dspId, list) ;

    DBC_Require (IS_VALID_PROCID(dspId)) ;
    DBC_Require (list != NULL) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [dspId] == TRUE) ;

    mplistState = &(LDRV_MPLIST_State [dspId]) ;

    temp = SWAP_LONG (DSP_addrConvert (dspId,
                                       (Uint32) &((list)->head),
                                       GppToDsp),
                      mplistState->wordSwap) ;

    if ( ((Uint32) (list)->head.next) == temp) {
        retVal = TRUE ;
    }

    TRC_1LEAVE ("LDRV_MPLIST_isEmpty", retVal) ;

    return retVal ;
}


/** ============================================================================
 *  @func   LDRV_MPLIST_clear
 *
 *  @desc   Clears a statically created list and translates its contents to
 *          work with DSP address space.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_clear (IN     ProcessorId dspId,
                   IN     List *      list )
{
    LDRV_MPLIST_Object * mplistState ;

    TRC_2ENTER ("LDRV_MPLIST_clear", dspId, list) ;

    DBC_Require (IS_VALID_PROCID(dspId)) ;
    DBC_Require (list != NULL) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [dspId] == TRUE) ;

    mplistState = &(LDRV_MPLIST_State [dspId]) ;

    list->head.next = (ListElement *)
                      SWAP_LONG (DSP_addrConvert (dspId,
                                                  (Uint32) &(list->head),
                                                  GppToDsp),
                                 mplistState->wordSwap) ;
    list->head.prev = list->head.next ;

    TRC_0LEAVE ("LDRV_MPLIST_clear") ;
}

/** ============================================================================
 *  @func   LDRV_MPLIST_getHead
 *
 *  @desc   Gets the head pointer from the list while working on the list object
 *          and element fields in DSP address space.If the head element is from
 *          pool memory, it does the  invalidate and writeback operations on the
 *          element. If the element is from a non pool shared memory, invalidate
 *          writeback operations  are not performed.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_getHead (IN     ProcessorId    dspId,
                     IN     List *         list,
                     OUT    ListElement ** headElement )
{
    PoolId               poolId         = POOL_INVALIDID;
    LDRV_MPLIST_Object * mplistState ;
    ListElement *        temp   ;

    TRC_3ENTER ("LDRV_MPLIST_getHead", dspId, list, headElement) ;

    DBC_Require (IS_VALID_PROCID(dspId)) ;
    DBC_Require (list        != NULL) ;
    DBC_Require (headElement != NULL) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [dspId] == TRUE) ;

    mplistState = &(LDRV_MPLIST_State [dspId]) ;

    if (LDRV_MPLIST_isEmpty (dspId, list) == TRUE) {
        *headElement = NULL ;
    }
    else {
        *headElement = (ListElement *)
                       DSP_addrConvert (dspId,
                                        SWAP_LONG (((Uint32) (list->head.next)),
                                                   mplistState->wordSwap),
                                        DspToGpp) ;
        /* Get the pool Id of the Head element */
        LDRV_POOL_getPoolId (dspId,
                             *headElement,
                             AddrType_Knl,
                             &poolId);

        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_invalidate (poolId, *headElement, sizeof (ListElement)) ;
        }
        list->head.next = (ListElement *) (*headElement)->next ;
        temp = (ListElement *)
                 DSP_addrConvert (dspId,
                                  SWAP_LONG ((Uint32) ((*headElement)->next),
                                             mplistState->wordSwap),
                                  DspToGpp) ;

        LDRV_POOL_getPoolId (dspId,
                             temp,
                             AddrType_Knl,
                             &poolId );
        /* Invalidate only when (*headElement)->next is from  POOL memory*/
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_invalidate (poolId, temp, sizeof (ListElement)) ;
        }
        temp->prev = (ListElement *)
                     SWAP_LONG (DSP_addrConvert (dspId,
                                                 (Uint32) (&(list->head)),
                                                 GppToDsp),
                                                 mplistState->wordSwap) ;
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_writeback (poolId, temp, sizeof (ListElement)) ;
        }
    }

    TRC_0LEAVE ("LDRV_MPLIST_getHead") ;
}

/** ============================================================================
 *  @func   LDRV_MPLIST_putTail
 *
 *  @desc   Adds the specified element to the tail of the list while working on
 *          the list object and element fields in DSP address space.If the
 *          element is from pool memory, it does the  invalidate and writeback
 *          operations on the element. If the element is from a non pool shared
 *          memory, invalidate writeback operations  are not performed.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_putTail (IN     ProcessorId   dspId,
                     IN     List *        list,
                     IN     ListElement * element )
{
    PoolId               poolId         = POOL_INVALIDID ;
    LDRV_MPLIST_Object * mplistState ;
    ListElement *        temp   ;
    Uint32               intermediateState ;

    TRC_3ENTER ("LDRV_MPLIST_putTail", dspId, list, element) ;

    DBC_Require (IS_VALID_PROCID(dspId)) ;
    DBC_Require (list    != NULL) ;
    DBC_Require (element != NULL) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [dspId] == TRUE) ;

    mplistState = &(LDRV_MPLIST_State [dspId]) ;

    element->prev   = list->head.prev ;
    element->next   = (ListElement *)
                          SWAP_LONG (DSP_addrConvert (dspId,
                                                      (Uint32) &(list->head),
                                                      GppToDsp),
                                     mplistState->wordSwap) ;
    /* Get the poolId of the element to do write back */
     LDRV_POOL_getPoolId (dspId,
                          element,
                          AddrType_Knl,
                          &poolId );

    if (IS_VALID_POOLID (poolId)) {
        LDRV_POOL_writeback (poolId, element, sizeof (ListElement)) ;
    }

    intermediateState = DSP_addrConvert (dspId,
                                         (Uint32) element,
                                         GppToDsp) ;
    if (IS_VALID_POOLID (poolId)) {
        LDRV_POOL_writeback (poolId, element, sizeof (ListElement)) ;
    }

    list->head.prev = (ListElement *) SWAP_LONG ((Uint32)intermediateState,
                                                 mplistState->wordSwap) ;
    temp = (ListElement *)
                  DSP_addrConvert (dspId,
                                   SWAP_LONG ((Uint32) (element->prev),
                                              mplistState->wordSwap),
                                   DspToGpp) ;
    /* Get the  poolId of temp, to be able to invalidate and write back
     * properly
     */
    LDRV_POOL_getPoolId (dspId,
                         temp,
                         AddrType_Knl,
                         &poolId );
    if (IS_VALID_POOLID (poolId)) {
        LDRV_POOL_invalidate (poolId, temp, sizeof (ListElement)) ;
    }
    temp->next = list->head.prev ;
    if (IS_VALID_POOLID (poolId)) {
        LDRV_POOL_writeback (poolId, temp, sizeof (ListElement)) ;
    }

    TRC_0LEAVE ("LDRV_MPLIST_putTail") ;
}

/** ============================================================================
 *  @func   LDRV_MPLIST_removeElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element. This function works on
 *          the list object and element fields in DSP address space.If the
 *          element is from pool memory, it does the  invalidate and writeback
 *          operations on the element. If the element is from a non pool shared
 *          memory, invalidate writeback operations  are not performed.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_removeElement (IN     ProcessorId    dspId,
                           IN     List *         list,
                           IN     ListElement *  element )
{
    PoolId               poolId        = POOL_INVALIDID ;
    LDRV_MPLIST_Object * mplistState ;
    ListElement *        temp   ;

    TRC_4ENTER ("LDRV_MPLIST_removeElement", dspId, list, element, poolId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (list    != NULL) ;
    DBC_Require (element != NULL) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [dspId] == TRUE) ;

    mplistState = &(LDRV_MPLIST_State [dspId]) ;

    if (LDRV_MPLIST_isEmpty (dspId, list) == FALSE) {
        temp = (ListElement *)
                  DSP_addrConvert (dspId,
                                   SWAP_LONG ((Uint32) (element->prev),
                                              mplistState->wordSwap),
                                   DspToGpp) ;

        LDRV_POOL_getPoolId (dspId,
                             temp,
                             AddrType_Knl,
                             &poolId );
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_invalidate (poolId, temp, sizeof (ListElement)) ;
        }
        temp->next = element->next ;
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_writeback (poolId, temp, sizeof (ListElement)) ;
        }

        temp = (ListElement *)
                  DSP_addrConvert (dspId,
                                   SWAP_LONG ((Uint32) (element->next),
                                              mplistState->wordSwap),
                                   DspToGpp) ;
        LDRV_POOL_getPoolId (dspId,
                             temp,
                             AddrType_Knl,
                             &poolId );
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_invalidate (poolId, temp, sizeof (ListElement)) ;
        }
        temp->prev = element->prev ;
        if (IS_VALID_POOLID (poolId)) {
            LDRV_POOL_writeback (poolId, temp, sizeof (ListElement)) ;
        }
    }

    TRC_0LEAVE ("LDRV_MPLIST_removeElement") ;
}

/** ============================================================================
 *  @func   LDRV_MPLIST_getMemInfo
 *
 *  @desc   This function returns the memory information for the MPLIST region.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_getMemInfo (IN OUT MPLIST_MemInfo * mplistMemInfo)
{
    DSP_STATUS           status = DSP_SOK ;
    LDRV_MPLIST_Object * mplistState ;

    TRC_1ENTER ("LDRV_MPLIST_getMemInfo", mplistMemInfo) ;

    DBC_Require (mplistMemInfo != NULL) ;
    DBC_Require (IS_VALID_PROCID (mplistMemInfo->procId)) ;
    DBC_Assert  (LDRV_MPLIST_IsInitialized [mplistMemInfo->procId] == TRUE) ;

    mplistState  = &(LDRV_MPLIST_State [mplistMemInfo->procId]) ;

    mplistMemInfo->kernAddr = (Uint32) mplistState->ctrlPtr ;
    mplistMemInfo->physAddr = mplistState->physCtrlAddr ;
    mplistMemInfo->size     = mplistState->ctrlSize ;

    TRC_1LEAVE ("LDRV_MPLIST_getMemInfo", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
