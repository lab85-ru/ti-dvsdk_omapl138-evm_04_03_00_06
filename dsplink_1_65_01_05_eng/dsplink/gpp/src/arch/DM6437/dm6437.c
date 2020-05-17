/** ============================================================================
 *  @file   dm6437.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/
 *
 *  @desc   Implementation of DSP interface-component for the DM6437 arch.
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

#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

/*  ----------------------------------- API                         */
#include <proc.h>

/*  ----------------------------------- Link Driver                 */
#include <dsp.h>
#include <ldrv.h>
#include <ldrv_utils.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>
#include <dm6437_hal.h>
#if (DM6437_PHYINTERFACE == PCI_INTERFACE)
#include <dm6437_hal_pci.h>
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE) */
#if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE)
#include <dm6437_hal_vlynq.h>
#endif /* if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE) */


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
#define SET_FAILURE_REASON TRC_SetReason (status, FID_C_ARCH_DSP_INTF, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  SELF_LOOP_LENGTH
 *
 *  @desc   Length of the selfloop.
 *  ============================================================================
 */
#define SELF_LOOP_LENGTH        15

/** ============================================================================
 *  @const  DM6437_BOOTROMENTRY_POINT
 *
 *  @desc   Entry point of BOOTROM CODE.
 *  ============================================================================
 */
#define DM6437_BOOTROMENTRY_POINT  0x00100000


/** ----------------------------------------------------------------------------
 *  @func   DM6437_init
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
 *  @see    DM6437_setup, DM6437_exit
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DM6437_init (IN ProcessorId  dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_exit
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
 *  @see    DM6437_init
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DM6437_exit (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_start
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
 *  @see    DM6437_stop
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DM6437_start (IN ProcessorId  dspId,
              IN DSP_Object * dspState,
              IN Uint32       dspAddr) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_stop
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
 *  @see    DM6437_start
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DM6437_stop (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_idle
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
 *  @see    DM6437_start
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DM6437_idle (IN ProcessorId dspId, IN DSP_Object * dspState) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_intCtrl
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
DM6437_intCtrl (IN         ProcessorId       dspId,
                IN         DSP_Object     *  dspState,
                IN         Uint32            intId,
                IN         DSP_IntCtrlCmd    cmd,
                IN OUT     Pvoid             arg) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_read
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
DM6437_read (IN  ProcessorId  dspId,
             IN DSP_Object *  dspState,
             IN  Uint32       dspAddr,
             IN  Endianism    endianInfo,
             IN  Uint32       numBytes,
             OUT Uint8 *      buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_write
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
DM6437_write (IN ProcessorId  dspId,
              IN DSP_Object * dspState,
              IN Uint32       dspAddr,
              IN Endianism    endianInfo,
              IN Uint32       numBytes,
              IN Uint8 *      buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_addrConvert
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
DM6437_addrConvert (IN  ProcessorId        dspId,
                    IN DSP_Object *        dspState,
                    IN  Uint32             addr,
                    IN  DSP_AddrConvType   type) ;


/** ----------------------------------------------------------------------------
 *  @func   DM6437_control
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
DM6437_control (IN  ProcessorId dspId,
                IN DSP_Object * dspState,
                IN  Int32       cmd,
                    Pvoid       arg) ;


/** ============================================================================
 *  @name   DM6437_Interface
 *
 *  @desc   The interface table exposed for the DM642 architecture.
 *  ============================================================================
 */
DSP_Interface DM6437_Interface = {
    &DM6437_init,
    &DM6437_exit,
    &DM6437_start,
    &DM6437_stop,
    &DM6437_idle,
    &DM6437_intCtrl,
    &DM6437_read,
    &DM6437_write,
    &DM6437_addrConvert,
    &DM6437_control,
} ;



/*  ----------------------------------------------------------------------------
 *  @func   DM6437_isRangeValid
 *
 *  @desc   Checks if the specified DSP address falls within the valid range of
 *          addresses specified in the static configuration.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    dspAddr
 *              DSP address to be checked for valid range
 *  @arg    size
 *              size of area that must be in range
 *
 *  @ret    TRUE
 *              If dspAddr <-> (dspAddr + size) is in valid DSP address range
 *          FALSE
 *              Otherwise
 *
 *  @enter  dspId must be a valid DSP identifier.
 *          size must not be zero.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
DM6437_isRangeValid (IN  ProcessorId dspId, IN Uint32 dspAddr, IN Uint32 size) ;


/** ============================================================================
 *  @func   DM6437_init
 *
 *  @desc   Initializes the components required by DSP.
 *          Puts the DSP in selfloop.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_init (IN ProcessorId  dspId, IN DSP_Object * dspState)
{
    DSP_STATUS          status    = DSP_SOK ;
    Uint32              i         = 0       ;
    DM6437_HalObj *     halObject = NULL    ;
    DM6437_InitParams   initParams          ;
    LINKCFG_Dsp *       dspObj              ;
    LINKCFG_MemEntry *  memTable            ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_2ENTER ("DM6437_init", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        memTable = dspCfg->memTables [dspObj->memTableId] ;

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
            if (   (memTable [dspObj->arg3].shared == TRUE)
                && (memTable [dspObj->arg3].syncd  == FALSE)) {
                initParams.shMemEntry = dspObj->arg3 ;
            }

            /* Populate config parameters into the halobject */
            initParams.dspId   = dspId ;
            initParams.arg1    = dspObj->arg1 ;
            initParams.arg2    = dspObj->arg2 ;
            initParams.arg5    = dspObj->arg5 ;
            initParams.phyType = dspObj->arg4 ;

            status = DM6437_halInit ((Pvoid *) &halObject, (Pvoid) &initParams);
            if (DSP_SUCCEEDED (status)) {
                dspState->halObject = halObject ;

        /*  --------------------------------------------------------------------
         *  In case DM6437, when device is in pciboot mode. And when power is
         *  applied, GEM starts running with PLL bypass mode and starting PC
         *  value is ROM address (where bootrom code is written). This bootrom
         *  code programs the PLL to run at comparable frequency and then it
         *  loops for BOOTCMPLT register (BC bit) to be set. Once this bit is
         *  set. it reads DSPBOOTADDR register and jumps to address specificed
         *  in DSPBOOTADDR. (This is TRUE for local reset also)
         *  So we put the DSP in local reset and release so that previous state
         *  is cleared and it starts executing from BOOTROM code.
         *  --------------------------------------------------------------------
         */
                if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
                    || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
                    status = halObject->interface->pwrCtrl  (
                                                           halObject,
                                                           DSP_PwrCtrlCmd_Reset,
                                                           NULL) ;
                    if (DSP_SUCCEEDED (status)) {
                        status = halObject->interface->bootCtrl (
                                            halObject,
                                            DSP_BootCtrlCmd_SetEntryPoint,
                                            (Pvoid) DM6437_BOOTROMENTRY_POINT);
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }

                    if (DSP_SUCCEEDED (status)) {
                        status = halObject->interface->bootCtrl (
                                                halObject,
                                                DSP_BootCtrlCmd_SetBootComplete,
                                                NULL) ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }

                    if (DSP_SUCCEEDED (status)) {
                        status = halObject->interface->pwrCtrl (
                                                       halObject,
                                                       DSP_PwrCtrlCmd_Release,
                                                       NULL);
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }

                if (DSP_SUCCEEDED (status)) {
                    for (i = 0 ; i < dspObj->memEntries ; i++) {
                        /* If the configured GPP virtual address is invalid,
                         * get the actual address from HAL object.
                         */
                        if (   (memTable [i].shared == TRUE)
                            && (memTable [i].syncd  == FALSE)) {
                            memTable[i].gppVirtAddr = halObject->shMemVirtAddr ;
                            memTable[i].physAddr    = halObject->shMemPhysAddr ;
                        }
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }

                /* Enable the DDR & EDMA */
                if (DSP_SUCCEEDED (status)) {
                    if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
                        || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)) {
                        status = halObject->interface->pwrCtrl (
                                                    halObject,
                                                    DSP_PwrCtrlCmd_PeripheralUp,
                                                    NULL) ;
                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("DM6437_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_exit
 *
 *  @desc   Finalizes the DSP sub-component and unmaps the memory mapped in
 *          DSP_init function.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_exit (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS          status    = DSP_SOK ;
    LINKCFG_Dsp *       dspObj ;
    DM6437_HalObj *     halObject = NULL    ;
    LINKCFG_MemEntry *  memTable ;
    Uint32              i ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_2ENTER ("DM6437_exit", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        halObject = (DM6437_HalObj *)dspState->halObject ;
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
            /* Ensure that the DSP is in <semi>reset state when LINK goes out */
            status = halObject->interface->bootCtrl (
                                             halObject,
                                             DSP_BootCtrlCmd_SetEntryPoint,
                                             (Pvoid) DM6437_BOOTROMENTRY_POINT);
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status = halObject->interface->bootCtrl (
                                                halObject,
                                                DSP_BootCtrlCmd_SetBootComplete,
                                                NULL) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status = halObject->interface->pwrCtrl  (halObject,
                                                     DSP_PwrCtrlCmd_Release,
                                                     NULL);
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        memTable  = dspCfg->memTables [dspObj->memTableId] ;

        for (i = 0 ; i < dspObj->memEntries ; i++) {
            if ((memTable [i].gppVirtAddr) != 0) {
                memTable [i].gppVirtAddr = (Uint32) -1 ;
            }
        }

        DM6437_halExit ((Pvoid *) &halObject) ;
    }

    TRC_1LEAVE ("DM6437_exit", status) ;

    return status ;
}

/** ============================================================================
 *  @func   DM6437_start
 *
 *  @desc   Causes DSP to start execution from the given DSP address by doing
 *          following steps :
 *              1. Writing the DSP executable start address to a predefined
 *                 location.
 *              2. Sending a command to DSP which will enable it to start
 *                 executing from the start address.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_start (IN ProcessorId dspId, IN DSP_Object * dspState, IN Uint32 dspAddr)
{
    DSP_STATUS          status  = DSP_SOK    ;
    DM6437_HalObj *     halObject = NULL    ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Dsp *       dspObj ;
    Uint32              entryPtAddrHi  = 0x00000000u ;
    Uint32              entryPtAddrLo  = 0x00000000u ;
    Uint32              startOpCodeLo  = 0x0000002Au ;
    Uint32              startOpCodeHi  = 0x0000006Au ;
    Uint32              noOp           = 0x00400000u ;
    Uint32              branch         = 0x00000362u ;
    Uint32              resetVector ;
    LINKCFG_Ips *       ipsTable                    ;
    LINKCFG_LinkDrv *   linkDrv                     ;

    TRC_3ENTER ("DM6437_start", dspId, dspState, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        halObject = (DM6437_HalObj *)dspState->halObject ;
        linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable = &(dspCfg->ipsTables [linkDrv->ipsTableId][0]) ;
        /* Specify the DSP boot address in the boot config register */
        if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
            status = halObject->interface->pwrCtrl  (halObject,
                                                     DSP_PwrCtrlCmd_Reset,
                                                     NULL) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            resetVector = dspObj->resetVector ;
            /* Write the branch instruction to at the boot address to branch
             * to _c_int00
             */
            entryPtAddrHi  = dspAddr >> 16u ;
            entryPtAddrLo  = dspAddr & 0xFFFFu ;
            startOpCodeHi |= (entryPtAddrHi << 7u) ;
            startOpCodeLo |= (entryPtAddrLo << 7u) ;
            status = halObject->interface->write (halObject,
                                                  (Uint32) dspObj->resetVector,
                                                  4u,
                                                  (Char8 *)&noOp) ;
            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                             halObject,
                                             (Uint32) dspObj->resetVector + 0x4,
                                             4u,
                                             (Char8 *)&startOpCodeLo) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                             halObject,
                                             (Uint32) dspObj->resetVector + 0x8,
                                             4u,
                                             (Char8 *)&startOpCodeHi) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                             halObject,
                                             (Uint32) dspObj->resetVector + 0xC,
                                             4u,
                                             (Char8 *)&branch) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                            halObject,
                                            (Uint32) dspObj->resetVector + 0x10,
                                            4u,
                                            (Char8 *)&noOp) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                            halObject,
                                            (Uint32) dspObj->resetVector + 0x14,
                                            4u,
                                            (Char8 *)&noOp) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                            halObject,
                                            (Uint32) dspObj->resetVector + 0x18,
                                            4u,
                                            (Char8 *)&noOp) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                            halObject,
                                            (Uint32) dspObj->resetVector + 0x1C,
                                            4u,
                                            (Char8 *)&noOp) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->write (
                                            halObject,
                                            (Uint32) dspObj->resetVector + 0x20,
                                            4u,
                                            (Char8 *)&noOp) ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Release the DSP from reset */
            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->bootCtrl (
                                                  halObject,
                                                  DSP_BootCtrlCmd_SetEntryPoint,
                                                 (Pvoid) resetVector);
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->bootCtrl (
                                                halObject,
                                                DSP_BootCtrlCmd_SetBootComplete,
                                                NULL);
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                status = halObject->interface->pwrCtrl (halObject,
                                                        DSP_PwrCtrlCmd_Release,
                                                        NULL);
            }
            else {
                SET_FAILURE_REASON ;
            }
        }

         if ( (DSP_SUCCEEDED (status)) &&
             (dspObj->doDspCtrl == DSP_BootMode_NoBoot)) {
             status = halObject->interface->intCtrl ((Pvoid) halObject,
                                                     DSP_IntCtrlCmd_Send,
                                                     ipsTable->dspIntId,
                                                     NULL) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("DM6437_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_stop
 *
 *  @desc   Causes DSP to stop execution by doing following steps :
 *              1. Writing 0 to the predefined location.
 *              2. Sending a command to DSP which will make it run in selfloop,
 *                 waiting for a command from GPP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_stop (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS          status = DSP_SOK ;
    DM6437_HalObj *     halObject = NULL ;
    LINKCFG_Dsp *       dspObj            ;
    LINKCFG_DspConfig * dspCfg            ;

    TRC_2ENTER ("DM6437_stop", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj = dspCfg->dspObject ;
        if (   (dspObj->doDspCtrl == DSP_BootMode_Boot_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_NoPwr)
            || (dspObj->doDspCtrl == DSP_BootMode_NoLoad_Pwr)
            || (dspObj->doDspCtrl == DSP_BootMode_Boot_NoPwr)) {
            halObject = (DM6437_HalObj *)dspState->halObject ;
            status = halObject->interface->pwrCtrl (halObject,
                                                    DSP_PwrCtrlCmd_Reset,
                                                    NULL) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status = halObject->interface->bootCtrl (
                                            halObject,
                                            DSP_BootCtrlCmd_SetEntryPoint,
                                            (Pvoid) DM6437_BOOTROMENTRY_POINT) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status = halObject->interface->bootCtrl (
                                                halObject,
                                                DSP_BootCtrlCmd_SetBootComplete,
                                                NULL) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status = halObject->interface->pwrCtrl (halObject,
                                                    DSP_PwrCtrlCmd_Release,
                                                    NULL);
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("DM6437_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_idle
 *
 *  @desc   Causes DSP to stop execution by doing following steps :
 *              1. Writing 0 to the predefined location.
 *              2. Sending a command to DSP which will make it run in selfloop,
 *                 waiting for a command from GPP.
 *          This function provides hook for power handling. Instead of making
 *          the DSP run the 'self loop', power handling can be written
 *          as part of 'idle code'.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_idle (IN ProcessorId dspId, IN DSP_Object * dspState)
{
    DSP_STATUS      status = DSP_SOK ;

    TRC_2ENTER ("DM6437_idle", dspId, dspState) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DM6437_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_intCtrl
 *
 *  @desc   Performs the specified DSP interrupt control activity.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_intCtrl (IN         ProcessorId       dspId,
                IN         DSP_Object *      dspState,
                IN         Uint32            intId,
                IN         DSP_IntCtrlCmd    cmd,
                IN OUT     Pvoid             arg)
{
    DSP_STATUS      status = DSP_SOK ;
    DM6437_HalObj * halObject = NULL ;

    TRC_5ENTER ("DM6437_intCtrl", dspId, dspState, intId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    if (IS_VALID_PROCID (dspId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        halObject = (DM6437_HalObj *)dspState->halObject ;
        status = halObject->interface->intCtrl (halObject,
                                                cmd,
                                                intId,
                                                arg) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DM6437_intCtrl", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_read
 *
 *  @desc   Reads data from DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_read (IN  ProcessorId  dspId,
             IN  DSP_Object * dspState,
             IN  Uint32       dspAddr,
             IN  Endianism    endianInfo,
             IN  Uint32       numBytes,
             OUT Uint8 *      buffer)
{
    DSP_STATUS      status = DSP_SOK ;
    DM6437_HalObj * halObject = NULL ;

    TRC_6ENTER ("DM6437_read",
                dspId,
                dspState,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;

    if (   (IS_VALID_PROCID (dspId) == FALSE)
        || (numBytes == 0)
        || (buffer == NULL)
        || (   (endianInfo != Endianism_Big)
            && (endianInfo != Endianism_Little)
            && (endianInfo != Endianism_Default))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (((DM6437_HalObj *)dspState->halObject)->isMapActive == TRUE) {
        status = DSP_EPENDING ;
        SET_FAILURE_REASON ;
    }
    else {
        halObject = (DM6437_HalObj *)dspState->halObject ;
        if (DM6437_isRangeValid (dspId, dspAddr, numBytes)) {
            status = halObject->interface->read (halObject,
                                                 dspAddr,
                                                 numBytes,
                                                 (Char8 *) buffer) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_ERANGE ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DM6437_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_write
 *
 *  @desc   Writes data to DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_write (IN ProcessorId  dspId,
              IN DSP_Object * dspState,
              IN Uint32       dspAddr,
              IN Endianism    endianInfo,
              IN Uint32       numBytes,
              IN Uint8 *      buffer)
{
    DSP_STATUS      status  = DSP_SOK ;
    DM6437_HalObj * halObject = NULL ;

    TRC_6ENTER ("DM6437_write",
                dspId,
                dspState,
                dspAddr,
                endianInfo,
                numBytes,
                buffer) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;
    DBC_Require (buffer != NULL) ;

    if (   (IS_VALID_PROCID (dspId) == FALSE)
        || (buffer == NULL)
        || (   (endianInfo != Endianism_Big)
            && (endianInfo != Endianism_Little)
            && (endianInfo != Endianism_Default))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (((DM6437_HalObj *)dspState->halObject)->isMapActive == TRUE) {
        status = DSP_EPENDING ;
        SET_FAILURE_REASON ;
    }
    else {
        halObject = (DM6437_HalObj *)dspState->halObject ;
        if (DM6437_isRangeValid (dspId, dspAddr, numBytes)) {
            status = halObject->interface->write (halObject,
                                                  dspAddr,
                                                  numBytes,
                                                  (Char8 *) buffer) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_ERANGE ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DM6437_write", status) ;

    return status ;
}


/*  ============================================================================
 *  @func   DM6437_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
DM6437_addrConvert (IN  ProcessorId        dspId,
                    IN  DSP_Object *       dspState,
                    IN  Uint32             addr,
                    IN  DSP_AddrConvType   type)
{
    Uint32              convAddr = (Uint32) ADDRMAP_INVALID ;
    Bool                found    = FALSE ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_MemEntry *  memEntry ;
    Uint32              byteAddr ;
    Uint32              i ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_4ENTER ("DM6437_addrConvert", dspId, dspState, addr, type) ;

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
    }

    TRC_2PRINT (TRC_LEVEL1,
                "DSP_addrConvert addr [0x%x] convAddr [0x%x]\n",
                addr,
                convAddr) ;

    TRC_0LEAVE ("DM6437_addrConvert") ;

    return convAddr ;
}


/** ============================================================================
 *  @func   DM6437_control
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_control (IN  ProcessorId dspId,
                IN  DSP_Object * dspState,
                IN  Int32       cmd,
                    Pvoid       arg)
{
    DSP_STATUS      status    = DSP_SOK ;
    DM6437_HalObj * halObject = NULL    ;
    DspDmaArgs *    dspDmaArgs          ;
    Uint32          temp                ;

    TRC_3ENTER ("DSP_control", dspId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dspState->halObject != NULL) ;

    halObject = (DM6437_HalObj *)dspState->halObject ;
    switch (cmd)
    {
        case PROC_CTRL_CMD_UNMAP_DSPMEM:
        {
            status = halObject->interface->mapCtrl (halObject,
                                                   DSP_MapCtrlCmd_Unmap,
                                                   0,
                                                   0,
                                                   NULL,
                                                   NULL,
                                                   &temp) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        break ;

        case PROC_CTRL_CMD_MAP_DSPMEM:
        {
            status = halObject->interface->mapCtrl (
                                      halObject,
                                      DSP_MapCtrlCmd_Map,
                                      ((ProcMemMapInfo *) arg)->dspAddr,
                                      ((ProcMemMapInfo *) arg)->size,
                                      &temp,
                                      (&((ProcMemMapInfo *) arg)->mappedAddr),
                                      (&((ProcMemMapInfo *) arg)->mappedSize)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        break ;

        case DSP_DmaCtrlCmd_GppToDsp:
        {
            dspDmaArgs = (DspDmaArgs *) arg ;
            status = halObject->interface->writeDMA (halObject,
                                                     dspDmaArgs->srcAddr,
                                                     dspDmaArgs->dstAddr,
                                                     dspDmaArgs->size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        break ;

        case DSP_DmaCtrlCmd_DspToGpp:
        {
            dspDmaArgs = (DspDmaArgs *) arg ;
            status = halObject->interface->readDMA (halObject,
                                                    dspDmaArgs->srcAddr,
                                                    dspDmaArgs->dstAddr,
                                                    dspDmaArgs->size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        break ;

        default:
        {
            status = DSP_ENOTIMPL ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DM6437_control", status) ;

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_isRangeValid
 *
 *  @desc   Checks if the specified DSP address falls within the valid range of
 *          addresses specified in the static configuration.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
DM6437_isRangeValid (IN  ProcessorId dspId, IN Uint32 dspAddr, IN Uint32 size)
{
    Bool                status = FALSE ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_MemEntry *  memEntry ;
    Uint32              i ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_3ENTER ("DM6437_isRangeValid", dspId, dspAddr, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (size != 0) ;

    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;

    for (i = 0 ; (status == FALSE) && (i < dspObj->memEntries) ; i++) {
        memEntry = &(dspCfg->memTables [dspObj->memTableId][i]) ;
        if (IS_RANGE_VALID (dspAddr,
                            memEntry->dspVirtAddr,
                            (  memEntry->dspVirtAddr
                             + memEntry->size))) {

            if (IS_RANGE_VALID (dspAddr + size,
                                memEntry->dspVirtAddr,
                                (  memEntry->dspVirtAddr
                                 + memEntry->size))) {
                status = TRUE ;
            }
            break ;
        }
    }

    TRC_1LEAVE ("DM6437_isRangeValid", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
