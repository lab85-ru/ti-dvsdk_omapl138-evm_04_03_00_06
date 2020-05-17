/** ============================================================================
 *  @file   da8xxgem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DA8XXGEM/
 *
 *  @desc   Implementation of DSP sub-component.
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
#include <gpptypes.h>
#include <dsplink.h>
#include <archdefs.h>
#include <errbase.h>
#include <_bitops.h>
#include <_intobject.h>
#include <_safe.h>

#include <loaderdefs.h>
#include <_dspdefs.h>
#include <_linkdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Headers                */
#include <mem_os.h>
#include <mem.h>

/*  ----------------------------------- Link Driver                 */
#include <dsp.h>
#include <ldrv.h>
#include <ldrv_utils.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <da8xxgem_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_DSP_INTF


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_DSP_INTF, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_init
 *
 *  @desc   Resets the DSP and initializes the components required by DSP.
 *          Puts the DSP in RESET state.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EVALUE
 *              Invalid DSP MMU endianism configuration.
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DA8XXGEM_setup, DA8XXGEM_exit
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_init (IN ProcessorId  dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_exit
 *
 *  @desc   Resets the DSP and puts it into IDLE Mode.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DA8XXGEM_init
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_exit (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_start
 *
 *  @desc   Causes DSP to start execution from the given DSP address.
 *          DSP is put to STARTED state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              Address to start execution from.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DA8XXGEM_stop
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_start (IN ProcessorId  dspId,
                  IN DSP_Object * dspState,
                  IN Uint32       dspAddr) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_stop
 *
 *  @desc   Stops execution on DSP.
 *          DSP is put to STOPPED state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DA8XXGEM_start
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_stop (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_idle
 *
 *  @desc   Idles the DSP.
 *          DSP is put to IDLE state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DA8XXGEM_start
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_idle (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_intCtrl
 *
 *  @desc   Perform the specified DSP interrupt control activity.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    intId
 *              Interrupt ID.
 *  @arg    cmd
 *              Interrupt control command to be performed.
 *  @arg    arg
 *              Optional input/output argument specific to each control command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          isrObj must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    DSP_start
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_intCtrl (IN         ProcessorId       dspId,
                   IN         DSP_Object     *  dspState,
                   IN         Uint32            intId,
                   IN         DSP_IntCtrlCmd    cmd,
                   IN OUT     Pvoid             arg) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_read
 *
 *  @desc   Reads data from DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              DSP address to read from.
 *  @arg    endianInfo
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to read.
 *  @arg    buffer
 *              Buffer to hold the read data.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          numBytes must be a valid pointer.
 *          buffer must be a valid pointer.
 *
 *  @leave  On successful completion, holds the number of bytes read.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_read (IN  ProcessorId  dspId,
                IN  DSP_Object * dspState,
                IN  Uint32       dspAddr,
                IN  Endianism    endianInfo,
                IN  Uint32       numBytes,
                OUT Uint8 *      buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_write
 *
 *  @desc   Writes data to DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    dspAddr
 *              DSP address to write to.
 *  @arg    endianInfo
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to write.
 *  @arg    buffer
 *              Buffer containing data to be written.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          buffer must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_write (IN ProcessorId  dspId,
                 IN DSP_Object * dspState,
                 IN Uint32       dspAddr,
                 IN Endianism    endianInfo,
                 IN Uint32       numBytes,
                 IN Uint8 *      buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    addr
 *              Address to be converted. If DSP address, the addr parameter
 *              reflects the DSP MADU address.
 *  @arg    type
 *              Type of address conversion.
 *
 *  @ret    Converted address.
 *              Operation successfully completed.
 *          ADDRMAP_INVALID.
 *              Specified address is not in mapped range.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_AddrConvType
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Uint32
DA8XXGEM_addrConvert (IN  ProcessorId        dspId,
                       IN  DSP_Object *       dspState,
                       IN  Uint32             addr,
                       IN  DSP_AddrConvType   type) ;


/** ----------------------------------------------------------------------------
 *  @func   DA8XXGEM_control
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspState
 *              DSP state Object.
 *  @arg    cmd
 *              Command id.
 *  @arg    arg
 *              Optional argument for the specified command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments specified.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_control (IN  ProcessorId  dspId,
                   IN  DSP_Object * dspState,
                   IN  Int32        cmd,
                       Pvoid        arg) ;


/** ============================================================================
 *  @name   DA8XXGEM_Interface
 *
 *  @desc   The interface table exposed for the DSP component.
 *  ============================================================================
 */
DSP_Interface DA8XXGEM_Interface = {
    &DA8XXGEM_init,
    &DA8XXGEM_exit,
    &DA8XXGEM_start,
    &DA8XXGEM_stop,
    &DA8XXGEM_idle,
    &DA8XXGEM_intCtrl,
    &DA8XXGEM_read,
    &DA8XXGEM_write,
    &DA8XXGEM_addrConvert,
    &DA8XXGEM_control,
} ;


/** ============================================================================
 *  @func   DA8XXGEM_init
 *
 *  @desc   Initializes the DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_init (IN ProcessorId  dspId, IN DSP_Object * dspState)
{
    DSP_STATUS          status = DSP_SOK ;
    DA8XXGEM_HalObj *  halObj  = NULL   ;
    LINKCFG_DspConfig * dspCfg ;
    DA8XXGEM_InitParams initParams       ;
    MemMapInfo          mapInfo          ;
    LINKCFG_Dsp *       dspObj           ;
    LINKCFG_MemEntry *  memTable         ;
    Uint32              i                ;

    TRC_2ENTER ("DA8XXGEM_init", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        memTable  = dspCfg->memTables [dspObj->memTableId] ;

        if (     (dspObj->doDspCtrl != DSP_BootMode_Boot_Pwr)
             &&  (dspObj->doDspCtrl != DSP_BootMode_NoLoad_NoPwr)
             &&  (dspObj->doDspCtrl != DSP_BootMode_NoLoad_Pwr)
             &&  (dspObj->doDspCtrl != DSP_BootMode_NoBoot)
             &&  (dspObj->doDspCtrl != DSP_BootMode_Boot_NoPwr)) {
            /* Check if the doDspCtrl is valid. */
            PRINT_Printf ("Configuration error:"
                          " Incorrect DSP doDspCtrl specified [0x%x]\n",
                          dspObj->doDspCtrl) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            for (i = 0 ;
                 (i < dspObj->memEntries) && DSP_SUCCEEDED (status) ;
                 i++) {
                /* If the configured GPP virtual address is invalid, get the actual
                 * address by mapping the physical address into GPP kernel memory
                 * space.
                 */
                if (memTable [i].gppVirtAddr == (Uint32) -1) {
                    mapInfo.src  = memTable [i].physAddr ;
                    mapInfo.size = memTable [i].size ;
                    mapInfo.memAttrs = MEM_UNCACHED ;

                    status = MEM_Map (&mapInfo) ;
                    if (DSP_SUCCEEDED (status)) {
                        memTable [i].gppVirtAddr = mapInfo.dst ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Populate config parameters into the halobject */
                initParams.dspId   = dspId ;

                status = DA8XXGEM_halInit (&dspState->halObject,
                                           (Pvoid) &initParams) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else  {
                    halObj = (DA8XXGEM_HalObj *) dspState->halObject ;
                }
            }
            if (DSP_SUCCEEDED (status)) {
                if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)) {
                    status = halObj->interface->pwrCtrl ((Pvoid) halObj,
                                                         DSP_PwrCtrlCmd_PowerUp,
                                                         NULL) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
                    /* Put the DPS in  local reset mode */
                    status = halObj->interface->pwrCtrl ((Pvoid) halObj,
                                                         DSP_PwrCtrlCmd_Reset,
                                                         NULL);
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
    }

    TRC_1LEAVE ("DA8XXGEM_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_exit
 *
 *  @desc   Finalizes the DSP sub-component. Either of following can be
 *          implemeted here based on application requirement:
 *          1. Reset the DSP as it it no longer required by application.
 *          2. Call DA8XXGEM_idle () to let the DSP run in 'self loop'.
 *          3. Let the DSP run free without modifying its execution state.
 *
 *          Option 1 is implemented here.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_exit (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS          status = DSP_SOK ;
    MemUnmapInfo        unmapInfo        ;
    LINKCFG_Dsp *       dspObj           ;
    LINKCFG_MemEntry *  memTable         ;
    DA8XXGEM_HalObj *  halObj           ;
    LINKCFG_DspConfig * dspCfg           ;
    Uint32              i                ;

    TRC_2ENTER ("DA8XXGEM_exit", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        memTable  = dspCfg->memTables [dspObj->memTableId] ;

        /* Check if DSP needs to be finalized. */
        if (dspState->halObject != NULL) {
            halObj = (DA8XXGEM_HalObj *) dspState->halObject ;

            if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
                || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)) {
                status = halObj->interface->pwrCtrl ((Pvoid) halObj,
                                                     DSP_PwrCtrlCmd_PowerDown,
                                                     NULL);
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            for (i = 0 ; i < dspObj->memEntries ; i++) {
                unmapInfo.addr = memTable [i].gppVirtAddr ;
                if (unmapInfo.addr != 0) {
                    status = MEM_Unmap (&unmapInfo) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                memTable [i].gppVirtAddr = (Uint32) -1 ;
            }

            DA8XXGEM_halExit (&dspState->halObject) ;
        }
    }

    TRC_1LEAVE ("DA8XXGEM_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_start
 *
 *  @desc   Causes DSP to start execution from the given DSP address.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_start (IN ProcessorId  dspId,
                  IN DSP_Object * dspState,
                  IN Uint32       dspAddr)
{
    DSP_STATUS          status         = DSP_SOK ;
    Uint32              gppAddr        = ADDRMAP_INVALID ;
    Uint32              entryPtAddrHi  = 0x00000000u ;
    Uint32              entryPtAddrLo  = 0x00000000u ;
    Uint32              startOpCodeLo  = 0x0000002Au ;
    Uint32              startOpCodeHi  = 0x0000006Au ;
    Uint32              noOp           = 0x00400000u ;
    Uint32              branch         = 0x00000362u ;
    Uint32              resetVector                 ;
    LINKCFG_Dsp *       dspObj                      ;
    DA8XXGEM_HalObj *   halObj                      ;
    LINKCFG_DspConfig * dspCfg                      ;
    LINKCFG_Ips *       ipsTable                    ;
    LINKCFG_LinkDrv *   linkDrv                     ;

    TRC_3ENTER ("DA8XXGEM_start", dspId, dspState, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        halObj   = (DA8XXGEM_HalObj *) dspState->halObject ;
        linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable = &(dspCfg->ipsTables [linkDrv->ipsTableId][0]) ;

                if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
            resetVector = dspObj->resetVector ;
            gppAddr = DA8XXGEM_addrConvert (
                                   dspId,
                                   dspState,
                                   BYTE_TO_MADU (resetVector, dspObj->maduSize),
                                   DspToGpp) ;
            if (gppAddr != ADDRMAP_INVALID) {
                /* Specify the DSP boot address in the boot config register */
                status = halObj->interface->bootCtrl (
                                                  (Pvoid) halObj,
                                                  DSP_BootCtrlCmd_SetEntryPoint,
                                                  (Pvoid) resetVector);

        if (DSP_SUCCEEDED (status)) {
                    /* Write the branch instruction to at the boot address to
                     * branch to _c_int00
                     */
                    entryPtAddrHi  = dspAddr >> 16 ;
                    entryPtAddrLo  = dspAddr & 0xFFFF ;
                    startOpCodeHi |= (entryPtAddrHi << 7) ;
                    startOpCodeLo |= (entryPtAddrLo << 7) ;
                    REG (gppAddr)  = startOpCodeLo ;
                    gppAddr += 4 ;
                    REG (gppAddr) = startOpCodeHi ;
                    gppAddr += 4 ;
                    REG (gppAddr) = branch        ;
                    gppAddr += 4 ;

                    /* Write 5 no-ops for pipeline flush */
                    REG (gppAddr) = noOp          ;
                    gppAddr += 4 ;
                    REG (gppAddr) = noOp          ;
                    gppAddr += 4 ;
                    REG (gppAddr) = noOp          ;
                    gppAddr += 4 ;
                    REG (gppAddr) = noOp          ;
                    gppAddr += 4 ;
                    REG (gppAddr) = noOp          ;
                    /* Release the DSP from reset */
                    status = halObj->interface->pwrCtrl ((Pvoid) halObj,
                                                         DSP_PwrCtrlCmd_Release,
                                                         NULL);
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                status = DSP_ERANGE ;
                PRINT_Printf ("Error: DSP reset vector is out of range.\n"
                              " Check /dsplink/config/all/CFG_<PLATFORM>.c for "
                              " RESETVECTOR.\n") ;
            }
        }
        if ( (DSP_SUCCEEDED (status)) &&
             (dspObj->doDspCtrl == DSP_BootMode_NoBoot)) {

            status = halObj->interface->intCtrl ((Pvoid) halObj,
                                                 DSP_IntCtrlCmd_Send,
                                                 ipsTable->dspIntId,
                                                 NULL) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            TRC_0PRINT (TRC_LEVEL1, "DSP started !\n") ;
        }
        else {
            SET_FAILURE_REASON ;
            TRC_0PRINT (TRC_LEVEL7, "DSP couldn't be started !\n") ;
        }
    }
    TRC_1LEAVE ("DA8XXGEM_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_stop
 *
 *  @desc   Stops execution on DSP.
 *          DSP transitions to STOPPED state after successful completion.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_stop (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS         status  = DSP_SOK ;
    DA8XXGEM_HalObj *   halObj            ;
    LINKCFG_Dsp *       dspObj            ;
    LINKCFG_DspConfig * dspCfg            ;

    TRC_2ENTER ("DA8XXGEM_stop", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj = dspCfg->dspObject ;
        halObj = (DA8XXGEM_HalObj *) dspState->halObject ;
        /* Release the DSP from reset */
        if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {

            status = halObj->interface->pwrCtrl ((Pvoid) halObj,
                                                 DSP_PwrCtrlCmd_Reset,
                                                 NULL);
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("DA8XXGEM_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_idle
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
DA8XXGEM_idle (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DA8XXGEM_idle", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DA8XXGEM_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_intCtrl
 *
 *  @desc   Performs the specified DSP interrupt control activity.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_intCtrl (IN         ProcessorId       dspId,
                  IN         DSP_Object *      dspState,
                  IN         Uint32            intId,
                  IN         DSP_IntCtrlCmd    cmd,
                  IN OUT     Pvoid             arg)
{
    DSP_STATUS          status = DSP_SOK ;
    DA8XXGEM_HalObj *  halObj           ;

    TRC_5ENTER ("DA8XXGEM_intCtrl", dspId, dspState, intId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        halObj = (DA8XXGEM_HalObj *) dspState->halObject ;
        status = halObj->interface->intCtrl ((Pvoid) halObj,
                                             cmd,
                                             intId,
                                             (Pvoid) arg) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DA8XXGEM_intCtrl", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_read
 *
 *  @desc   Read data from DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_read (IN  ProcessorId  dspId,
               IN DSP_Object *  dspState,
               IN  Uint32       dspAddr,
               IN  Endianism    endianInfo,
               IN  Uint32       numBytes,
               OUT Uint8 *      buffer)
{
    DSP_STATUS          status  = DSP_SOK ;
    Uint32              gppAddr = ADDRMAP_INVALID ;
    Uint8  *            dspPtr8 = NULL ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_6ENTER ("DA8XXGEM_read",
                dspId,
                dspState,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer   != NULL) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (   (IS_VALID_PROCID (dspId) == FALSE)
        || (numBytes == 0)
        || (buffer   == NULL)
        || (   (endianInfo != Endianism_Big)
            && (endianInfo != Endianism_Little)
            && (endianInfo != Endianism_Default))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;

        gppAddr = DA8XXGEM_addrConvert (
                                   dspId,
                                   dspState,
                                   BYTE_TO_MADU (dspAddr, dspObj->maduSize),
                                   DspToGpp) ;
        if (gppAddr != ADDRMAP_INVALID) {
            dspPtr8 = (Uint8 *)  gppAddr ;
            status = MEM_Copy (buffer,
                               dspPtr8,
                               numBytes,
                               Endianism_Default) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;

        }
        else {
            status = DSP_ERANGE ;
            SET_FAILURE_REASON ;
        }
#if defined (DDSP_PROFILE)
        DSP_State [dspId].dspStats.dataDspToGpp += numBytes ;
#endif /* defined (DDSP_PROFILE) */
    }

    TRC_1LEAVE ("DA8XXGEM_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_write
 *
 *  @desc   Write data to DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_write (IN ProcessorId   dspId,
                  IN DSP_Object *  dspState,
                  IN Uint32        dspAddr,
                  IN Endianism     endianInfo,
                  IN Uint32        numBytes,
                  IN Uint8 *       buffer)
{
    DSP_STATUS          status  = DSP_SOK ;
    Uint32              gppAddr = ADDRMAP_INVALID ;
    Uint8  *            dspPtr8 = NULL ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    Uint8               temp8_1 ;
    Uint8               temp8_2 ;
    Uint8               temp8_3 ;
    Uint8               temp8_4 ;
    Uint32              temp ;

    TRC_6ENTER ("DA8XXGEM_write",
                dspId,
                dspState,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (buffer != NULL) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (   (IS_VALID_PROCID (dspId) == FALSE)
        || (buffer == NULL)
        || (   (endianInfo != Endianism_Big)
            && (endianInfo != Endianism_Little)
            && (endianInfo != Endianism_Default))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;

        /*  --------------------------------------------------------------------
         *  Check if 'dspAddr' lies in 'Self Loop Area'
         *  --------------------------------------------------------------------
         */
        if (    ((dspAddr + numBytes) > dspObj->resetVector)
            &&  (dspAddr              < (   dspObj->resetVector
                                         +  dspObj->resetCodeSize))) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
        else {
            gppAddr = DA8XXGEM_addrConvert (
                                       dspId,
                                       dspState,
                                       BYTE_TO_MADU (dspAddr, dspObj->maduSize),
                                       DspToGpp) ;
            if (gppAddr != ADDRMAP_INVALID) {
                if (numBytes != sizeof(Uint32)) {
                    dspPtr8 = (Uint8 *)  gppAddr ;
                    status = MEM_Copy (dspPtr8,
                                        buffer,
                                        numBytes,
                                        Endianism_Default) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                }
                else  {
                     /* For 4 bytes, directly write as a Uint32 */
                    temp8_1 = ((Uint8 *)buffer)[0] ;
                    temp8_2 = ((Uint8 *)buffer)[1] ;
                    temp8_3 = ((Uint8 *)buffer)[2] ;
                    temp8_4 = ((Uint8 *)buffer)[3] ;
                    temp = (Uint32) (    ((Uint32) temp8_4 << 24)
                                     |   ((Uint32) temp8_3 << 16)
                                     |   ((Uint32) temp8_2 << 8)
                                     |   ((Uint32) temp8_1)) ;
                     *((Uint32*)gppAddr)      = temp ;

                }
            }
            else {
                status = DSP_ERANGE ;
                SET_FAILURE_REASON ;
            }
        }

#if defined (DDSP_PROFILE)
        if (DSP_SUCCEEDED (status)) {
            DSP_State [dspId].dspStats.dataGppToDsp += numBytes ;
        }
#endif /* defined (DDSP_PROFILE) */
    }

    TRC_1LEAVE ("DA8XXGEM_write", status) ;

    return status ;
}


/*  ============================================================================
 *  @func   DA8XXGEM_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
DA8XXGEM_addrConvert (IN  ProcessorId         dspId,
                        IN  DSP_Object *       dspState,
                        IN  Uint32             addr,
                        IN  DSP_AddrConvType   type)
{
    Uint32              convAddr = (Uint32) ADDRMAP_INVALID ;
    Bool                found    = FALSE ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_MemEntry *  memEntry ;
    Uint32              byteAddr ;
    Uint32              i ;

    TRC_4ENTER ("DA8XXGEM_addrConvert", dspId, dspState, addr, type) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;

    for (i = 0 ; (found == FALSE) && (i < dspObj->memEntries) ; i++) {
        memEntry = &(dspCfg->memTables [dspObj->memTableId][i]) ;
        if (type == GppToDsp) {
            if (IS_RANGE_VALID (addr,
                                memEntry->gppVirtAddr,
                                (  memEntry->gppVirtAddr
                                 + memEntry->size))) {
                found = TRUE ;
                convAddr = BYTE_TO_MADU ((  (addr - memEntry->gppVirtAddr)
                                          + memEntry->dspVirtAddr),
                                         dspObj->maduSize) ;
            }
        }
        else if (type == DspToGpp) {
            byteAddr = MADU_TO_BYTE (addr, dspObj->maduSize) ;
            if (IS_RANGE_VALID (byteAddr,
                                memEntry->dspVirtAddr,
                                (  memEntry->dspVirtAddr
                                 + memEntry->size))) {
                found = TRUE ;
                convAddr =    byteAddr
                           -  memEntry->dspVirtAddr
                           +  memEntry->gppVirtAddr ;
            }
        }
        else {
            /* Added for MISRAC compliance */
        }
    }

    TRC_2PRINT (TRC_LEVEL1,
                "DA8XXGEM_addrConvert addr [0x%x] convAddr [0x%x]\n",
                addr,
                convAddr) ;

    TRC_0LEAVE ("DA8XXGEM_addrConvert") ;

    return convAddr ;
}


/** ============================================================================
 *  @func   DA8XXGEM_control
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_control (IN  ProcessorId dspId,
                    IN DSP_Object * dspState,
                    IN  Int32       cmd,
                        Pvoid       arg)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("DA8XXGEM_control", dspId, dspState, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    status = DSP_ENOTIMPL ;

    TRC_1LEAVE ("DA8XXGEM_control", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
