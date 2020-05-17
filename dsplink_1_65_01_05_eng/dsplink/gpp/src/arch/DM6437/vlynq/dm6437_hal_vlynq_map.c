/** ============================================================================
 *  @file   dm6437_hal_vlynq_map.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/vlynq/
 *
 *  @desc   MAP control module.
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
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>
#include <dm6437_hal_vlynq_map.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_MAP


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_MAP, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   DM6437_halVlynqMapCtrl
 *
 *  @desc   Mapping controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halVlynqMapCtrl (IN      Pvoid           halObj,
                        IN      DSP_MapCtrlCmd  cmd,
                        IN      Uint32          dspBufAddr,
                        IN      Uint32          size,
                        OUT     Uint32 *        bufVirtPtr,
                        OUT     Uint32 *        bufPhysAddr,
                        OUT     Uint32 *        mappedSize)
{
    DSP_STATUS         status    = DSP_SOK ;
    DM6437_vlynqRegs * pvlynqRegs  = NULL  ;
    DM6437_HalObj *    halObject = (DM6437_HalObj *) halObj ;
    Uint32             availBytes          ;

    TRC_6ENTER ("DM6437_halVlynqMapCtrl",
                halObj,
                cmd,
                dspBufAddr,
                size,
                bufVirtPtr,
                bufPhysAddr) ;

    DBC_Require (NULL != halObject) ;
    DBC_Require (NULL != mappedSize) ;

    pvlynqRegs  = (DM6437_vlynqRegs *) (  halObject->vlynq.region3Addr
                                        + DM6437_PEERVLYNQREG_BASE) ;

    switch (cmd) {
        case DSP_MapCtrlCmd_Map:
        {
            availBytes = (  DM6437_VLYNQ_WINDOWSIZE
                          - pvlynqRegs->LOCAL_RAMS1
                          - pvlynqRegs->LOCAL_RAMS2
                          - pvlynqRegs->LOCAL_RAMS3) ;
            if ((availBytes > 0) && (size > 0)) {
                halObject->vlynq.prevUserAddr = pvlynqRegs->LOCAL_RAMO4 ;
                halObject->vlynq.prevUserSize = pvlynqRegs->LOCAL_RAMS4 ;
                pvlynqRegs->LOCAL_RAMO4 = dspBufAddr ;
                pvlynqRegs->LOCAL_RAMS4 = size ;
                *bufVirtPtr  = halObject->vlynq.region4Addr ;
                *bufPhysAddr = halObject->vlynq.region4Addr ;
                halObject->vlynq.region4Size = size ;
                if (availBytes >= size) {
                    *mappedSize = size ;
                 }
                 else {
                    *mappedSize = availBytes ;
                 }
            }
            else {
                status = DSP_EMEMORY ;
                SET_FAILURE_REASON ;
            }
        }
        break ;

        case DSP_MapCtrlCmd_Unmap:
        {
            pvlynqRegs->LOCAL_RAMO4 = halObject->vlynq.prevUserAddr ;
            pvlynqRegs->LOCAL_RAMS4 = halObject->vlynq.prevUserSize ;
        }
        break ;

        case DSP_MapCtrlCmd_SetShared:
        {
            pvlynqRegs->LOCAL_RAMO1 = dspBufAddr ;
            pvlynqRegs->LOCAL_RAMS1 = size ;
            halObject->vlynq.region1Size = size ;
            *bufVirtPtr  = halObject->vlynq.region1Addr ;
            *bufPhysAddr = halObject->vlynq.region1Addr ;
            /* Correct the regions */
            halObject->vlynq.region2Addr =  halObject->vlynq.region1Addr
                                          + halObject->vlynq.region1Size ;
            halObject->vlynq.region3Addr =  halObject->vlynq.region2Addr
                                          + halObject->vlynq.region2Size ;
            halObject->vlynq.region4Addr =  halObject->vlynq.region3Addr
                                          + halObject->vlynq.region3Size ;
            *mappedSize = size ;
        }
        break ;

        default:
        {
            /* Unsupported interrupt control command */
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        break ;
    }

    TRC_1LEAVE ("DM6437_halVlynqIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
