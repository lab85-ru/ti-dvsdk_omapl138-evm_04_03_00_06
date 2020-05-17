/** ============================================================================
 *  @file   dsp.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/
 *
 *  @desc   Implementation of DSP wrapper.
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
#include <_dsplink.h>
#include <linkcfgdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem_os.h>
#include <mem.h>

/*  ----------------------------------- Link Driver                 */
#include <dsp.h>
#include <ldrv.h>
#include <ldrv_utils.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */



/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_DSP


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_DSP, __LINE__)
#else
#define SET_FAILURE_REASON   { }
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @name   DSP_State
 *
 *  @desc   State object for the DSP component containing all information
 *          required by it.
 *  ============================================================================
 */
STATIC DSP_Object DSP_State [MAX_DSPS] ;


/** ============================================================================
 *  @func   DSP_moduleInit
 *
 *  @desc   Initializes the DSP module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
DSP_moduleInit (Void)
{
    TRC_0ENTER ("DSP_moduleInit") ;

    TRC_0LEAVE ("DSP_moduleInit") ;
}


/** ============================================================================
 *  @func   DSP_moduleExit
 *
 *  @desc   Finalizes the DSP module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
DSP_moduleExit (Void)
{
    TRC_0ENTER ("DSP_moduleExit") ;

    TRC_0LEAVE ("DSP_moduleExit") ;
}


/** ============================================================================
 *  @func   DSP_init
 *
 *  @desc   Initializes the DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_init (IN ProcessorId  dspId, IN DSP_Interface * interface)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DSP_init", dspId, interface) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (interface != NULL) ;

#if defined (DDSP_PROFILE)
    DSP_State [dspId].dspStat.dataGppToDsp = 0 ;
    DSP_State [dspId].dspStat.dataDspToGpp = 0 ;
    DSP_State [dspId].dspStat.intsGppToDsp = 0 ;
    DSP_State [dspId].dspStat.intsDspToGpp = 0 ;
#endif /* if defined (DDSP_PROFILE) */

    DSP_State [dspId].dspInterface = interface ;
    status = (DSP_State [dspId].dspInterface->init) (dspId, &DSP_State [dspId]) ;

    TRC_1LEAVE ("DSP_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_exit
 *
 *  @desc   Finalizes the DSP sub-component. Either of following can be
 *          implemeted here based on application requirement:
 *          1. Reset the DSP as it it no longer required by application.
 *          2. Call DSP_idle () to let the DSP run in 'self loop'.
 *          3. Let the DSP run free without modifying its execution state.
 *
 *          Option 1 is implemented here.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_exit (IN ProcessorId dspId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DSP_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    status = (DSP_State [dspId].dspInterface->exit) (dspId, &DSP_State [dspId]) ;

    TRC_1LEAVE ("DSP_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_start
 *
 *  @desc   Causes DSP to start execution from the given DSP address.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_start (IN ProcessorId dspId, IN Uint32 dspAddr)
{
    DSP_STATUS          status         = DSP_SOK ;

    TRC_2ENTER ("DSP_start", dspId, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    status = (DSP_State [dspId].dspInterface->start) (dspId,
                                                   &DSP_State [dspId],
                                                   dspAddr) ;

    TRC_1LEAVE ("DSP_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_stop
 *
 *  @desc   Stops execution on DSP.
 *          DSP transitions to STOPPED state after successful completion.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_stop (IN ProcessorId dspId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DSP_stop", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    status = (DSP_State [dspId].dspInterface->stop) (dspId, &DSP_State [dspId]) ;

    TRC_1LEAVE ("DSP_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_idle
 *
 *  @desc   Places the DSP into idle.
 *          This function provides hook for power handling. Instead of an empty
 *          function, power handling can be written as part of 'idle code'.

 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_idle (IN ProcessorId dspId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DSP_idle", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    status = (DSP_State [dspId].dspInterface->idle) (dspId, &DSP_State [dspId]) ;

    TRC_1LEAVE ("DSP_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_intCtrl
 *
 *  @desc   Performs the specified DSP interrupt control activity.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_intCtrl (IN         ProcessorId       dspId,
             IN         Uint32            intId,
             IN         DSP_IntCtrlCmd    cmd,
             IN OUT     Pvoid             arg)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("DSP_intCtrl", dspId, intId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

#if defined (DDSP_PROFILE)
    if (cmd == DSP_IntCtrlCmd_Send) {
        DSP_State [dspId].dspStat.intsGppToDsp++ ;
    }
    if (cmd == DSP_IntCtrlCmd_Clear) {
        DSP_State [dspId].dspStat.intsDspToGpp++ ;
    }
#endif /* if defined (DDSP_PROFILE) */

    status = (DSP_State [dspId].dspInterface->intCtrl) (dspId,
                                                     &DSP_State [dspId],
                                                     intId,
                                                     cmd,
                                                     arg) ;

    TRC_1LEAVE ("DSP_intCtrl", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_read
 *
 *  @desc   Read data from DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_read (IN  ProcessorId  dspId,
          IN  Uint32       dspAddr,
          IN  Endianism    endianInfo,
          IN  Uint32       numBytes,
          OUT Uint8 *      buffer)
{
    DSP_STATUS     status  = DSP_SOK ;

    TRC_5ENTER ("DSP_read",
                dspId,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer   != NULL) ;

#if defined (DDSP_PROFILE)
    DSP_State [dspId].dspStat.dataDspToGpp += numBytes ;
#endif /* if defined (DDSP_PROFILE) */

    status = (DSP_State [dspId].dspInterface->read) (dspId,
                                                  &DSP_State [dspId],
                                                  dspAddr,
                                                  endianInfo,
                                                  numBytes,
                                                  buffer) ;

    TRC_1LEAVE ("DSP_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DSP_write
 *
 *  @desc   Write data to DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_write (IN ProcessorId dspId,
           IN Uint32      dspAddr,
           IN Endianism   endianInfo,
           IN Uint32      numBytes,
           IN Uint8 *     buffer)
{
    DSP_STATUS     status  = DSP_SOK ;

    TRC_5ENTER ("DSP_write",
                dspId,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer   != NULL) ;

#if defined (DDSP_PROFILE)
    DSP_State [dspId].dspStat.dataGppToDsp += numBytes ;
#endif /* if defined (DDSP_PROFILE) */

    status = (DSP_State [dspId].dspInterface->write) (dspId,
                                                   &DSP_State [dspId],
                                                   dspAddr,
                                                   endianInfo,
                                                   numBytes,
                                                   buffer) ;

    TRC_1LEAVE ("DSP_write", status) ;

    return status ;
}


/*  ============================================================================
 *  @func   DSP_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
DSP_addrConvert (IN  ProcessorId        dspId,
                 IN  Uint32             addr,
                 IN  DSP_AddrConvType   type)
{
    Uint32             convAddr = (Uint32) ADDRMAP_INVALID ;

    TRC_3ENTER ("DSP_addrConvert", dspId, addr, type) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    convAddr = (DSP_State [dspId].dspInterface->addrConvert) (dspId,
                                                           &DSP_State [dspId],
                                                           addr,
                                                           type) ;

    TRC_2PRINT (TRC_LEVEL1,
                "DSP_addrConvert addr [0x%x] convAddr [0x%x]\n",
                addr,
                convAddr) ;

    TRC_0LEAVE ("DSP_addrConvert") ;

    return convAddr ;
}


/** ============================================================================
 *  @func   DSP_control
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_control (IN  ProcessorId dspId,
             IN  Int32       cmd,
                 Pvoid       arg)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("DSP_control", dspId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

#if defined (DDSP_PROFILE)
    if (cmd == DSP_DmaCtrlCmd_GppToDsp) {
        DSP_State [dspId].dspStat.dmaGppToDsp++ ;
    }
    if (cmd == DSP_DmaCtrlCmd_DspToGpp) {
        DSP_State [dspId].dspStat.dmaDspToGpp++ ;
    }
#endif /* if defined (DDSP_PROFILE) */

    status = (DSP_State [dspId].dspInterface->control) (dspId,
                                                     &DSP_State [dspId],
                                                     cmd,
                                                     arg) ;

    TRC_1LEAVE ("DSP_control", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   DSP_instrument
 *
 *  @desc   Gets the instrumentation information related to the specified
 *          DSP object.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_instrument (IN  ProcessorId dspId, OUT DSP_Stats * retVal)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DSP_instrument", dspId, retVal) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (retVal != NULL) ;

    if (!IS_VALID_PROCID (dspId) || (retVal == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *retVal = DSP_State [dspId].dspStats ;
    }

    TRC_1LEAVE ("DSP_instrument", status) ;

    return status ;
}

#endif /* if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DSP_debug
 *
 *  @desc   Prints out debug information of the specified DSP object.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
DSP_debug (IN  ProcessorId dspId)
{
    LINKCFG_Dsp *       dspObj = NULL ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_1ENTER ("DSP_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (!IS_VALID_PROCID (dspId)) {
        TRC_1PRINT (TRC_LEVEL7, "Invalid DSP ID %d\n", dspId) ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;

        /*  --------------------------------------------------------------------
         *  Print the DSP component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "DSP component state information for DSP ID %d:\n",
                    dspId) ;
         
        (void) dspObj ;                       

        /*  --------------------------------------------------------------------
         *  Print the DSP object configuration information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "DSP object configuration information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    name            = [%s]\n",   dspObj->name) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspArch         = [0x%x]\n", dspObj->dspArch) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    autoStart       = [0x%x]\n", dspObj->autoStart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    execName        = [%s]\n",   dspObj->execName) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    doDspCtrl       = [%s]\n",   dspObj->doDspCtrl) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    resumeAddr      = [%s]\n",   dspObj->resumeAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    resetVector     = [0x%x]\n", dspObj->resetVector) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    resetCodeSize   = [0x%x]\n", dspObj->resetCodeSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    maduSize        = [%d]\n",   dspObj->maduSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    endian          = [%d]\n",   dspObj->endian) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    wordSwap        = [%d]\n",   dspObj->wordSwap) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    memTableId      = [%d]\n",   dspObj->memTableId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    memEntries      = [%d]\n",   dspObj->memEntries) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    linkDrvId       = [%d]\n",   dspObj->linkDrvId) ;
    }

    TRC_0LEAVE ("DSP_debug") ;
}
#endif /* if defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
