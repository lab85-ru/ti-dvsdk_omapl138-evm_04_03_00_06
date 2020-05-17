/** ============================================================================
 *  @file   idm.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implementation of functions for ID Manager component.
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
#include <_safe.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>
#include <idm.h>



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_GEN_IDM


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_GEN_IDM, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @macro  MAX_IDM_OBJECTS
 *
 *  @desc   Maximum number of objects supported by the IDM component.
 *  ============================================================================
 */
#define MAX_IDM_OBJECTS      32u

/** ============================================================================
 *  @macro  IDM_INVALID_KEY
 *
 *  @desc   Invalid key used for identifying the IDM object
 *  ============================================================================
 */
#define IDM_INVALID_KEY      (Uint32) 0xFF


/** ============================================================================
 *  @name   IDM_Id
 *
 *  @desc   IDM ID identified by a unique ID key per IDM object.
 *
 *  @field  idKey
 *              ID key associated with the ID to be returned.
 *  @field  refCount
 *              Reference count indicating the number of clients that have
 *              acquired this ID.
 *  ============================================================================
 */
typedef struct IDM_Id_tag {
    Char8     idKey [DSP_MAX_STRLEN] ;
    Uint16    refCount ;
} IDM_Id ;


/** ============================================================================
 *  @name   IDM_Object
 *
 *  @desc   IDM object identified by a unique key
 *
 *  @field  key
 *              Base ID supported by this IDM object.
 *  @field  baseId
 *              Base ID supported by this IDM object.
 *  @field  maxIds
 *              Maximum number of IDs supported by this IDM objects.
 *  @field  idArray
 *              Dynamically allocated ID array indicating acquired and released
 *              IDs. The value of the ID is its index in the array added to the
 *              base ID.
 *  ============================================================================
 */
typedef struct IDM_Object_tag {
    Uint32    key ;
    Uint16    baseId ;
    Uint16    maxIds ;
    IDM_Id *  idArray ;
} IDM_Object ;

/** ============================================================================
 *  @name   IDM_State
 *
 *  @desc   State object for the IDM component
 *
 *  @field  isInitialized
 *              Indicates whether the IDM component is initialized.
 *  @field  idmObjs
 *              Array of IDM objects.
 *  ============================================================================
 */
typedef struct IDM_State_tag {
    Bool        isInitialized ;
    IDM_Object  idmObjs [MAX_IDM_OBJECTS] ;
} IDM_State ;


/** ============================================================================
 *  @name   IDM_StateObj
 *
 *  @desc   State object containing all global information for the IDM component
 *  ============================================================================
 */
STATIC IDM_State IDM_StateObj = {
    FALSE
} ;


/** ============================================================================
 *  @func   IDM_init
 *
 *  @desc   Initializes the IDM component
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_init (Void)
{
    DSP_STATUS status = DSP_SOK ;
    Bool       toInit = FALSE ;
    Uint32     i ;

    TRC_0ENTER ("IDM_init") ;

    SYNC_ProtectionStart () ;
    if (IDM_StateObj.isInitialized == FALSE) {
        IDM_StateObj.isInitialized = TRUE ;
        toInit = TRUE ;
    }
    else {
        status = DSP_SALREADYSETUP ;
    }
    SYNC_ProtectionEnd () ;

    if (toInit == TRUE) {
        for (i = 0 ; i < MAX_IDM_OBJECTS ; i++) {
            IDM_StateObj.idmObjs [i].key     = IDM_INVALID_KEY ;
            IDM_StateObj.idmObjs [i].idArray = NULL ;
        }
    }

    DBC_Ensure (IDM_StateObj.isInitialized == TRUE) ;

    TRC_1LEAVE ("IDM_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IDM_exit
 *
 *  @desc   Finalizes the IDM component
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_exit (Void)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     i ;

    TRC_0ENTER ("IDM_exit") ;

    DBC_Require (IDM_StateObj.isInitialized == TRUE) ;

    if (IDM_StateObj.isInitialized == TRUE) {
        /* Delete any unfreed IDM objects. */
        for (i = 0 ; i < MAX_IDM_OBJECTS ; i++) {
            if (IDM_StateObj.idmObjs [i].key != IDM_INVALID_KEY) {
                status = FREE_PTR (IDM_StateObj.idmObjs [i].idArray) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }

                /* Reset the key to indicate deleted object. */
                IDM_StateObj.idmObjs [i].key = IDM_INVALID_KEY ;
            }
        }

        IDM_StateObj.isInitialized = FALSE ;
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (IDM_StateObj.isInitialized == FALSE) ;

    TRC_1LEAVE ("IDM_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IDM_create
 *
 *  @desc   Creates an IDM object identified based on a unique key specified by
 *          the user.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_create (IN Uint32 key, IN IDM_Attrs * attrs)
{
    DSP_STATUS status = DSP_ERESOURCE ;
    Bool       found  = FALSE ;
    Uint32     i ;
    Uint32     j ;

    TRC_2ENTER ("IDM_create", key, attrs) ;

    DBC_Require (attrs != NULL) ;
    DBC_Require (IDM_StateObj.isInitialized == TRUE) ;

    if (attrs == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        for (i = 0 ; ((i < MAX_IDM_OBJECTS) && (found == FALSE)) ; i++) {
            /* Find a free IDM object. */
            if (IDM_StateObj.idmObjs [i].key == IDM_INVALID_KEY) {
                found = TRUE ;
                IDM_StateObj.idmObjs [i].key    = key ;
                IDM_StateObj.idmObjs [i].baseId = attrs->baseId ;
                IDM_StateObj.idmObjs [i].maxIds = attrs->maxIds ;
                status = MEM_Alloc ((Void **) &IDM_StateObj.idmObjs [i].idArray,
                                    (sizeof (IDM_Id) * attrs->maxIds),
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Initialize all IDs to indicate that they are free to be
                     * acquired.
                     */
                    for (j = 0 ; j < attrs->maxIds ; j++) {
                        IDM_StateObj.idmObjs [i].idArray [j].idKey [0] = '\0' ;
                        IDM_StateObj.idmObjs [i].idArray [j].refCount  = 0 ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("IDM_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IDM_delete
 *
 *  @desc   Deletes an IDM object identified based on a unique key specified by
 *          the user.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_delete (IN Uint32 key)
{
    DSP_STATUS status = DSP_ENOTFOUND ;
    Bool       found  = FALSE ;
    Uint32     i ;

    TRC_1ENTER ("IDM_delete", key) ;

    DBC_Require (IDM_StateObj.isInitialized == TRUE) ;

    for (i = 0 ; ((i < MAX_IDM_OBJECTS) && (found == FALSE)) ; i++) {
        /* Find the IDM object to be deleted based on the key. */
        if (IDM_StateObj.idmObjs [i].key == key) {
            found = TRUE ;
            status = FREE_PTR (IDM_StateObj.idmObjs [i].idArray) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* Reset the key to indicate deleted object. */
            IDM_StateObj.idmObjs [i].key = IDM_INVALID_KEY ;
        }
    }

    TRC_1LEAVE ("IDM_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IDM_acquireId
 *
 *  @desc   Acquires a free ID for the specified IDM object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_acquireId (IN Uint32 key, IN Pstr idKey, OUT Uint32 * id)
{
    DSP_STATUS   status      = DSP_SOK ;
    Bool         idFound     = FALSE ;
    Bool         freeIdFound = FALSE ;
    IDM_Object * idmObject   = NULL ;
    Uint32       freeId      = 0 ;
    Uint32       i ;
    Uint32       j ;
    Int32        cmpResult ;

    TRC_3ENTER ("IDM_acquireId", key, idKey, id) ;

    DBC_Require (id    != NULL) ;
    DBC_Require (idKey != NULL) ;
    DBC_Require (IDM_StateObj.isInitialized == TRUE) ;

    if ((id == NULL) || (idKey == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
		SYNC_ProtectionStart () ;
        for (i = 0 ;
                (i < MAX_IDM_OBJECTS)
             && DSP_SUCCEEDED (status)
             && (idmObject == NULL) ;
             i++) {
            /* Find the IDM object based on the key. */
            if (IDM_StateObj.idmObjs [i].key == key) {
                idmObject = &(IDM_StateObj.idmObjs [i]) ;
                /* Check if the specified idKey already exists. Also find and
                 * store the first free ID in case the idKey does not exist.
                 */
                for (j = 0 ; ((j < idmObject->maxIds) && (idFound == FALSE)) ; j++) {
                    /* Check for free key. */
                    if (    (freeIdFound == FALSE)
                        &&  (idmObject->idArray [j].idKey [0] == '\0')) {
                        /* Found a free key. Store the free slot to use in case
                         * existing key is not found.
                         */
                        freeId = j ;
                        freeIdFound = TRUE ;
                    }
                    else {
                        status = GEN_Strcmp (idmObject->idArray [j].idKey,
                                             idKey,
                                             &cmpResult) ;
                        /* Validity of parameters is ensured for success. */
                        DBC_Assert (DSP_SUCCEEDED (status)) ;
                        if (cmpResult == 0) {
                            /* Found existing idKey. Increment refCount and
                             * return the ID.
                             */
                            idmObject->idArray [j].refCount++ ;
                            *id = idmObject->baseId + j ;
                            idFound = TRUE ;
                            status = DSP_SEXISTS ;
                        }
                    }
                }

                if (idFound == FALSE) {
                    if (freeIdFound == FALSE) {
                        /* No free ID found. All IDs have been used. */
                        status = DSP_ERESOURCE ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* Existing key was not found. Now use the free ID and
                         * set the specified key.
                         */
                        status = GEN_Strcpyn (idmObject->idArray [freeId].idKey,
                                              idKey,
                                              DSP_MAX_STRLEN) ;
                        /* Validity of parameters is ensured for success. */
                        DBC_Assert (DSP_SUCCEEDED (status)) ;
                        idmObject->idArray [freeId].refCount = 1 ;
                        *id = idmObject->baseId + freeId ;
                    }
                }
            }
        }

        if (idmObject == NULL) {
            /* Object corresponding to key not found */
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }

		SYNC_ProtectionEnd () ;
    }

    TRC_1LEAVE ("IDM_acquireId", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IDM_releaseId
 *
 *  @desc   Releases the specified ID for the specified IDM object
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IDM_releaseId (IN Uint32 key, IN Uint32 id)
{
    DSP_STATUS   status = DSP_SOK ;
    IDM_Object * idmObject = NULL ;
    Uint32       i ;

    TRC_2ENTER ("IDM_releaseId", key, id) ;

    DBC_Require (IDM_StateObj.isInitialized == TRUE) ;
	SYNC_ProtectionStart () ;
    for (i = 0 ;
            (i < MAX_IDM_OBJECTS)
         && DSP_SUCCEEDED (status)
         && (idmObject == NULL) ;
         i++) {
        /* Find the IDM object based on the key. */
        if (IDM_StateObj.idmObjs [i].key == key) {
            idmObject = &(IDM_StateObj.idmObjs [i]) ;

            /* Get the index of the ID in the array. */
            id = id - idmObject->baseId ;
            /* Decrement refCount and check if the ID needs to be released. */
            idmObject->idArray [id].refCount-- ;
            if (idmObject->idArray [id].refCount == 0) {
                idmObject->idArray [id].idKey [0] = '\0' ;
                status = DSP_SFREE ;
            }
        }
    }

    if (idmObject == NULL) {
        /* Object corresponding to key not found */
        status = DSP_ENOTFOUND ;
        SET_FAILURE_REASON ;
    }

	SYNC_ProtectionEnd () ;

    TRC_1LEAVE ("IDM_releaseId", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
