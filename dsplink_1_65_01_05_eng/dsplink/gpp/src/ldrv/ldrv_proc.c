/** ============================================================================
 *  @file   ldrv_proc.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Functionality implementation of the PROC sub-component in LDRV.
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


/*  ------------------------------------- DSP/BIOS Link             */
#include <dsplink.h>
#include <_dsplink.h>
#include <linkcfgdefs.h>
#include <loaderdefs.h>
#include <procdefs.h>
#if defined (POOL_COMPONENT)
#include <_pooldefs.h>
#endif /* if defined (POOL_COMPONENT) */

/*  ------------------------------------- Trace & Debug             */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ------------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif

/*  ------------------------------------- ARCH Headers              */
#include <dsp.h>

/*  ------------------------------------- OSAL Headers              */
#include <sync.h>
#include <dpc.h>
#include <kfile.h>

/*  ------------------------------------- Generic Functions         */
#include <gen_utils.h>

/*  ------------------------------------- Link Driver               */
#include <cfgmap.h>
#include <ldrv.h>
#if !(defined (ONLY_PROC_COMPONENT))
#include <ldrv_drv.h>
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */
#include <ldrv_proc.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_PROC


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_PROC, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_PROC_Object
 *
 *  @desc   This structure defines the LDRV PROC object, which contains all
 *          state information required by the LDRV PROC component.
 *
 *  @field  procId
 *              Processor identifier.
 *  @field  interface
 *              Pointer to the link driver interface table for this DSP.
 *  @field  loaderIntf
 *              Pointer to the interface table for the DSP executable loader.
 *  @field  kfileIntf
 *              Pointer to the interface table for the KFILE module.
 *  @field  dspState
 *              Holds the current state of the DSP.
 *  @field  procStats
 *              Instrumentation information for the PROC component.
 *              Defined only if profiling is enabled.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_PROC_Object_tag {
    Uint32             procId ;
    DSP_Interface *    interface ;
    LOADER_Interface * loaderIntf ;
    KFILE_Interface *  kfileIntf ;
    PROC_State         dspState ;
#if defined (DDSP_PROFILE)
    PROC_Stats         procStats ;
#endif /* if defined (DDSP_PROFILE) */
} LDRV_PROC_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_PROC_State
 *
 *  @desc   State object for the LDRV_PROC component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_PROC_Object LDRV_PROC_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_PROC_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_PROC_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_PROC_moduleInit
 *
 *  @desc   This function initializes the LDRV_PROC module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_moduleInit (Void)
{
    DSP_STATUS         status = DSP_SOK ;
    Uint32             dspId ;

    TRC_0ENTER ("LDRV_PROC_moduleInit") ;

    DSP_moduleInit () ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flags for the DSP. */
        LDRV_PROC_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_PROC_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_moduleExit
 *
 *  @desc   This function finalizes the LDRV_PROC module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_PROC_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_PROC_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_PROC_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    DSP_moduleExit () ;

    TRC_1LEAVE ("LDRV_PROC_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_init
 *
 *  @desc   Allocates resources at GPP side that are required for using DSP.
 *          It also sets up the connection to DSP from the GPP and other
 *          associated peripheral hardware.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_init (IN ProcessorId dspId)
{
    DSP_STATUS          status = DSP_SOK  ;
    LDRV_PROC_Object *  procState  = NULL ;
    DSP_Interface *     interface = NULL  ;
    LINKCFG_Dsp *      dspObj ;
    Int32              strCmpResult ;
    Uint32             mapId ;
    LINKCFG_DspConfig * dspConfig ;

    TRC_1ENTER ("LDRV_PROC_init", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    if (LDRV_PROC_IsInitialized [dspId] == FALSE) {
        /* Initialize the IsInitialized flags for the DSP. */
        LDRV_PROC_IsInitialized [dspId] = FALSE ;

        procState = &(LDRV_PROC_State [dspId]) ;
        dspConfig = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspConfig->dspObject ;

        /* Initialize the state object. */
        procState->procId    = dspId ;
        procState->interface = NULL ;
        procState->dspState  = ProcState_Unknown ;

        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the DSP.
         *  --------------------------------------------------------------------
         */
        /* Check if the configured DSP is available and set the interface table
         * if the configuration is valid.
         */
        for (mapId = 0 ; mapId < CFGMAP_Config [dspId]->numDsps ; mapId++) {
            status = GEN_Strcmp (dspObj->name,
                                 CFGMAP_Config [dspId]->dspObjects [mapId].name,
                                 &strCmpResult) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (strCmpResult == 0) {
                interface = CFGMAP_Config [dspId]->dspObjects [mapId].interface ;
                break ;
            }
        }

        if (mapId == CFGMAP_Config [dspId]->numDsps) {
            /* Configured DSP is not available. */
            PRINT_Printf ("Configuration error:"
                          " Incorrect DSP name specified [%s]\n",
                          dspObj->name) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the configured loader is available and set the interface
             * table if the configuration is valid.
             */
            for (mapId = 0 ; mapId < CFGMAP_Config [dspId]->numLoaders ; mapId++) {
                status = GEN_Strcmp (dspObj->loaderName,
                                     CFGMAP_Config [dspId]->loaders [mapId].name,
                                     &strCmpResult) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                if (strCmpResult == 0) {
                    procState->loaderIntf =
                               CFGMAP_Config [dspId]->loaders [mapId].interface ;
                    procState->kfileIntf =
                                      CFGMAP_Config [dspId]->loaders [mapId].kfileIntf ;
                    break ;
                }
            }

            if (mapId == CFGMAP_Config [dspId]->numLoaders) {
                /* Configured loader is not available. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect loader name specified [%s]\n",
                              dspObj->loaderName) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (dspObj->dspArch != DspArch_Unknown)
                     &&  (dspObj->dspArch != DspArch_C55x)
                     &&  (dspObj->dspArch != DspArch_C64x_Bios5)
                     &&  (dspObj->dspArch != DspArch_C64x_Bios6)
                     &&  (dspObj->dspArch != DspArch_C64x)) {
                /* Check if the dspArch is valid. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect DSP architecture specified [0x%x]\n",
                              dspObj->dspArch) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (dspObj->autoStart != TRUE)
                     &&  (dspObj->autoStart != FALSE)) {
                /* Check if the autoStart is valid. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect DSP autoStart specified [0x%x]\n",
                              dspObj->autoStart) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (dspObj->endian != Endianism_Default)
                     &&  (dspObj->endian != Endianism_Big)
                     &&  (dspObj->endian != Endianism_Little)) {
                /* Check if the endian is valid. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect DSP endian specified [0x%x]\n",
                              dspObj->endian) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (dspObj->wordSwap != TRUE)
                     &&  (dspObj->wordSwap != FALSE)) {
                /* Check if the wordSwap is valid. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect DSP wordSwap specified [0x%x]\n",
                              dspObj->wordSwap) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (dspObj->memTableId >= dspConfig->numMemTables) {
                /* Check if the memTableId is in valid range. */
                PRINT_Printf ("Configuration error:"
                              " Incorrect DSP memTableId specified [0x%x]\n",
                              dspObj->memTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
    }

        /*  --------------------------------------------------------------------
         *  Setup and initialize the DSP.
         *  --------------------------------------------------------------------
         */
        if (DSP_SUCCEEDED (status)) {
            status = DSP_init (dspId, interface) ;
            if (DSP_SUCCEEDED (status)) {
                procState->dspState = ProcState_Reset ;
            }
            else {
                SET_FAILURE_REASON ;
                PRINT_Printf (" DSP_init status [0x%x] \n",status) ;
            }
        }

#if !(defined (ONLY_PROC_COMPONENT))
        if (DSP_SUCCEEDED (status)) {
            status = LDRV_DRV_init (dspId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                PRINT_Printf(" LDRV_DRV_init status [0x%x] \n",status) ;
            }
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        if (DSP_FAILED (status)) {
            procState->dspState = ProcState_Unknown ;
        }

#if defined (DDSP_PROFILE)
        if (DSP_SUCCEEDED (status)) {
            /* Increment the count of active links */
            procState->procStats.procData [dspId].activeLinks++ ;
        }
#endif
        LDRV_PROC_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_PROC_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_PROC_exit (dspId) ;
            LDRV_PROC_IsInitialized [dspId] = FALSE ;
        }
    }

    DBC_Ensure (   (   (DSP_SUCCEEDED (status))
                    && (procState->dspState == ProcState_Reset))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("LDRV_PROC_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_exit
 *
 *  @desc   Releases all the resources created on GPP side for specified DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_exit (IN ProcessorId dspId)
{
    DSP_STATUS         status       = DSP_SOK ;
    DSP_STATUS         tmpStatus    = DSP_SOK ;
    LDRV_PROC_Object * procState ;

    TRC_1ENTER ("LDRV_PROC_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;
    if (LDRV_PROC_IsInitialized [dspId] == TRUE) {
        if (LDRV_LinkCfgPtr != NULL) {

#if !(defined (ONLY_PROC_COMPONENT))
            status = LDRV_DRV_exit (dspId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

            tmpStatus = DSP_exit (dspId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    procState->dspState = ProcState_Unknown ;
                    SET_FAILURE_REASON ;
                }
                else {
                    procState->dspState = ProcState_Idle ;
                }
            }

        LDRV_PROC_IsInitialized [dspId] = FALSE ;
    }

    DBC_Ensure (   (   (DSP_SUCCEEDED (status))
                    && (procState->dspState == ProcState_Idle))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("LDRV_PROC_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_start
 *
 *  @desc   Starts execution of DSP from specified location.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_start (IN ProcessorId dspId, IN Uint32 dspAddr)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procState ;

    TRC_2ENTER ("LDRV_PROC_start", dspId, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (   (procState->dspState == ProcState_Loaded)
                 || (procState->dspState == ProcState_Stopped)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (   (procState->dspState == ProcState_Loaded)
        || (procState->dspState == ProcState_Stopped)) {
        /* Setup handshaking between the GPP and DSP. */
#if !(defined (ONLY_PROC_COMPONENT))
        status = LDRV_DRV_handshake (dspId, DRV_HandshakeSetup) ;
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        if (DSP_SUCCEEDED (status)) {
            status = DSP_start (dspId, dspAddr) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

#if !(defined (ONLY_PROC_COMPONENT))
        if (DSP_SUCCEEDED (status)) {
            /* Start handshaking between the GPP and DSP. */
            status = LDRV_DRV_handshake (dspId, DRV_HandshakeStart) ;
            if (DSP_SUCCEEDED (status)) {
                /* Wait for completion of handshaking between the GPP and DSP */
                status = LDRV_DRV_handshake (dspId, DRV_HandshakeCompl) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if !(defined (ONLY_PROC_COMPONENT)) */

        if (DSP_SUCCEEDED (status)) {
            procState->dspState = ProcState_Started ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   (   (DSP_SUCCEEDED (status))
                    && (procState->dspState == ProcState_Started))
                || (DSP_FAILED (status))) ;


    TRC_1LEAVE ("LDRV_PROC_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_stop
 *
 *  @desc   Stops the execution of DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_stop (IN ProcessorId dspId)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procState ;

    TRC_1ENTER ("LDRV_PROC_stop", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (   (procState->dspState == ProcState_Started)
                 || (procState->dspState == ProcState_Stopped)
                 || (procState->dspState == ProcState_Idle)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (   (procState->dspState == ProcState_Started)
        || (procState->dspState == ProcState_Stopped)
        || (procState->dspState == ProcState_Idle)) {
        if (LDRV_PROC_IsInitialized [dspId] == TRUE) {
            status = DSP_stop (dspId) ;
            if (DSP_FAILED (status)) {
                procState->dspState = ProcState_Unknown ;
                SET_FAILURE_REASON ;
            }
            else {
                procState->dspState = ProcState_Stopped ;
            }
        }
    }
    else {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   (   (DSP_SUCCEEDED (status))
                    && (procState->dspState == ProcState_Stopped))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("LDRV_PROC_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_idle
 *
 *  @desc   Puts the processor in idle mode, which means that read and write
 *          can be done to the processor.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_idle (IN ProcessorId dspId)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procState ;

    TRC_1ENTER ("LDRV_ProcState_Idle", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (procState->dspState != ProcState_Unknown) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (procState->dspState != ProcState_Unknown) {
        status = DSP_idle (dspId) ;
        if (DSP_FAILED (status)) {
            procState->dspState = ProcState_Unknown ;
            SET_FAILURE_REASON ;
        }
        else {
            procState->dspState = ProcState_Idle ;
        }
    }
    else {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   (   (DSP_SUCCEEDED (status))
                    && (procState->dspState == ProcState_Idle))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("LDRV_ProcState_Idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_read
 *
 *  @desc   Reads from the DSP's memory space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_read (IN     ProcessorId   dspId,
                IN     Uint32        dspAddr,
                IN     Endianism     endianInfo,
                IN     Uint32        numBytes,
                OUT    Uint8 *       buffer)
{
    DSP_STATUS         status = DSP_SOK ;
    Uint32             irqFlags         ;
    LDRV_PROC_Object * procState ;

    TRC_5ENTER ("LDRV_PROC_read",
                dspId,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (procState->dspState != ProcState_Unknown) ;
    DBC_Require (buffer != NULL) ;
    DBC_Require (numBytes != 0) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (procState->dspState != ProcState_Unknown) {
        DSP_intCtrl (dspId,
                     (Uint32) NULL,
                     DSP_IntCtrlCmd_Disable,
                     NULL) ;
        irqFlags = SYNC_SpinLockStart () ;
        status = DSP_read (dspId,
                           dspAddr,
                           endianInfo,
                           numBytes,
                           buffer) ;
        if (DSP_FAILED (status)) {
            procState->dspState = ProcState_Unknown ;
            SET_FAILURE_REASON ;
        }

#if defined (DDSP_PROFILE)
        if (DSP_SUCCEEDED (status)) {
            procState->procStats.procData [dspId].dataFromDsp += numBytes ;
        }
#endif /* defined (DDSP_PROFILE) */
        SYNC_SpinLockEnd (irqFlags) ;
        DSP_intCtrl (dspId,
                     (Uint32) NULL,
                     DSP_IntCtrlCmd_Enable,
                     NULL) ;
    }
    else {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_PROC_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_write
 *
 *  @desc   Writes to DSP's memory space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_write (IN  ProcessorId    dspId,
                 IN  Uint32         dspAddr,
                 IN  Endianism      endianInfo,
                 IN  Uint32         numBytes,
                 IN  Uint8 *        buffer)
{
    DSP_STATUS         status = DSP_SOK ;
    Uint32             irqFlags  ;
    LDRV_PROC_Object * procState ;

    TRC_5ENTER ("LDRV_PROC_write",
                dspId,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;


    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;
    DBC_Require (procState->dspState != ProcState_Unknown) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (procState->dspState != ProcState_Unknown) {
        DSP_intCtrl (dspId,
                     (Uint32) NULL,
                     DSP_IntCtrlCmd_Disable,
                     NULL) ;
        irqFlags = SYNC_SpinLockStart () ;
        status = DSP_write (dspId,
                            dspAddr,
                            endianInfo,
                            numBytes,
                            buffer) ;
        if (DSP_FAILED (status)) {
            procState->dspState = ProcState_Unknown ;
            SET_FAILURE_REASON ;
        }
#if defined (DDSP_PROFILE)
        else {
            procState->procStats.procData [dspId].dataToDsp += numBytes ;
        }
#endif /* defined (DDSP_PROFILE) */
        SYNC_SpinLockEnd (irqFlags) ;
        DSP_intCtrl (dspId,
                     (Uint32) NULL,
                     DSP_IntCtrlCmd_Enable,
                     NULL) ;
    }
    else {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_PROC_write", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
LDRV_PROC_addrConvert (IN  ProcessorId        dspId,
                       IN  Uint32             addr,
                       IN  DSP_AddrConvType   type)
{
    Uint32             address   = ADDRMAP_INVALID ;
    LDRV_PROC_Object * procState ;

    TRC_3ENTER ("LDRV_PROC_addrConvert",
                dspId,
                addr,
                type) ;


    DBC_Require (IS_VALID_PROCID (dspId)) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (procState->dspState != ProcState_Unknown) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    if (procState->dspState != ProcState_Unknown) {
        address = DSP_addrConvert (dspId,
                                   addr,
                                   type) ;
    }

    TRC_1LEAVE ("LDRV_PROC_addrConvert", address) ;

    return address ;
}


/** ============================================================================
 *  @func   LDRV_PROC_getState
 *
 *  @desc   Returns the current processor state.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_getState (IN ProcessorId dspId, OUT PROC_State * procState)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procStateObj ;

    TRC_2ENTER ("LDRV_PROC_getState", dspId, procState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    procStateObj = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (procState != NULL) ;

    *procState = procStateObj->dspState ;

    TRC_1LEAVE ("LDRV_PROC_getState", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_setState
 *
 *  @desc   Sets the current state of processor to the specified state.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_setState (IN ProcessorId dspId, IN PROC_State procState)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procStateObj ;

    TRC_2ENTER ("LDRV_PROC_setState", dspId, procState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    procStateObj = &(LDRV_PROC_State [dspId]) ;

    procStateObj->dspState = procState ;

    TRC_1LEAVE ("LDRV_PROC_setState", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_control (IN  ProcessorId dspId,
                   IN  Int32       cmd,
                       Pvoid       arg)
{
    DSP_STATUS         status = DSP_SOK ;

    TRC_3ENTER ("LDRV_PROC_control", dspId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    status = DSP_control (dspId, cmd, arg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_PROC_control", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_PROC_getLoader
 *
 *  @desc   Get the loader interface table.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_PROC_getLoader (IN  ProcessorId         procId,
                     OUT LOADER_Interface ** loaderIntf,
                     OUT KFILE_Interface **  kfileIntf)
{
    *loaderIntf = LDRV_PROC_State [procId].loaderIntf ;
    *kfileIntf  = LDRV_PROC_State [procId].kfileIntf ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_PROC_instrument
 *
 *  @desc   Gets the instrumentation information related to the specified
 *          DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_instrument (IN ProcessorId dspId, OUT PROC_Instrument * retVal)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_PROC_Object * procState ;

    TRC_2ENTER ("LDRV_PROC_instrument", dspId, retVal) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_PROC_IsInitialized [dspId] == TRUE) ;

    procState = &(LDRV_PROC_State [dspId]) ;

    DBC_Require (retVal != NULL) ;

    *retVal = procState->procStats.procData [dspId] ;

    TRC_1LEAVE ("LDRV_PROC_instrument", status) ;

    return status ;
}
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_PROC_debug
 *
 *  @desc   Prints out debug information of LDRV_PROC module.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_PROC_debug (IN ProcessorId dspId)
{
#if defined (TRACE_ENABLE)
    LDRV_PROC_Object * procState ;
#endif /* if defined (TRACE_ENABLE) */

    TRC_1ENTER ("LDRV_PROC_debug", dspId) ;

#if defined (TRACE_ENABLE)
    procState = &(LDRV_PROC_State [dspId]) ;
#endif /* if defined (TRACE_ENABLE) */

    TRC_2PRINT (TRC_LEVEL1,
                "Processor Number %d, Current State = %d\n",
                dspId,
                procState->dspState) ;

    TRC_0LEAVE ("LDRV_PROC_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   LDRV_PROC_isStarted
 *
 *  @desc   Check whether DSP is started or not.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_PROC_isStarted (IN  ProcessorId procId)
{
    DSP_STATUS status  = DSP_SOK ;
    PROC_State  procState ;

    if (LDRV_PROC_IsInitialized [procId] == TRUE) {
    LDRV_PROC_getState (procId, &procState) ;
    if (procState != ProcState_Started) {
            status = DSP_EWRONGSTATE ;
        }
    }
    else {
        status = DSP_EWRONGSTATE ;
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
