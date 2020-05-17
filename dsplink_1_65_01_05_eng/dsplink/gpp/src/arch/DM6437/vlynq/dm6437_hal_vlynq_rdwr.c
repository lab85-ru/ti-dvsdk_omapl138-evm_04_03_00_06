/** ============================================================================
 *  @file   dm6437_hal_vlynq_rdwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/vlynq/
 *
 *  @desc   Read Write module.
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

/*  ----------------------------------- OSAL Headers                */
#include <mem_os.h>
#include <mem.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>
#include <dm6437_hal_vlynq_rdwr.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_RDWR


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status,FID_C_ARCH_HAL_RDWR,__LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   DM6437_halVlynqRead
 *
 *  @desc   Reads a buffer from dsp memory (for now this is done as bytes)
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halVlynqRead (IN  Pvoid          halObj,
                     IN  Uint32         dspAddr,
                     IN  Uint32         cBytes,
                     OUT Char8 *        readBuffer)
{
    DSP_STATUS         status           = DSP_SOK    ;
    DM6437_vlynqRegs * pvlynqRegs       = NULL       ;
    DM6437_HalObj *    halObject        = (DM6437_HalObj *) halObj ;
    Char8 *            curHostPtr       = readBuffer ;
    Char8 *            dspPtr           = NULL       ;
    Uint32             rBytes           = 0          ;
    Uint32             orgPageBase                   ;

    TRC_4ENTER ("DM6437_halVlynqRead", halObj, dspAddr, cBytes, readBuffer) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (cBytes > 0) ;
    DBC_Require (readBuffer != NULL) ;

    pvlynqRegs  = (DM6437_vlynqRegs *) (  halObject->vlynq.region3Addr
                                        + DM6437_PEERVLYNQREG_BASE) ;

    /* Save the original page base address */
    orgPageBase = pvlynqRegs->LOCAL_RAMO2 ;

    while (cBytes > 0) {
        pvlynqRegs->LOCAL_RAMO2 = (dspAddr + rBytes) ;
        curHostPtr = ((Char8 *) ((Uint32) readBuffer + rBytes)) ;
        dspPtr     = (Char8 *) halObject->vlynq.region2Addr ;

        if (pvlynqRegs->LOCAL_RAMS2 >= cBytes) {
            MEM_Copy ((unsigned char *) curHostPtr,
                      (unsigned char *) dspPtr,
                      cBytes,
                      Endianism_Default) ;
            cBytes = 0 ;
        }
        else {
            rBytes += pvlynqRegs->LOCAL_RAMS2 ;
            MEM_Copy ((unsigned char *) curHostPtr,
                      (unsigned char *) dspPtr,
                      pvlynqRegs->LOCAL_RAMS2,
                      Endianism_Default) ;
            cBytes -= pvlynqRegs->LOCAL_RAMS2 ;
        }
    }

    /* Restore the original page base address */
    pvlynqRegs->LOCAL_RAMO2 = orgPageBase ;

    TRC_1LEAVE ("DM6437_halVlynqRead", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_halVlynqWrite
 *
 *  @desc   Writes a buffer to dsp memory (for now this is done as bytes)
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halVlynqWrite (IN  Pvoid          halObj,
                      IN  Uint32         dspAddr,
                      IN  Uint32         cBytes,
                      IN  Char8 *        writeBuffer)
{
    DSP_STATUS         status           = DSP_SOK     ;
    DM6437_vlynqRegs * pvlynqRegs       = NULL        ;
    DM6437_HalObj *    halObject        = (DM6437_HalObj *) halObj ;
    Char8 *            curHostPtr       = writeBuffer ;
    Char8 *            dspPtr           = NULL        ;
    Uint32             wBytes           = 0           ;
    Uint32             orgPageBase                    ;
    Uint8              temp8_1                        ;
    Uint8              temp8_2                        ;
    Uint8              temp8_3                        ;
    Uint8              temp8_4                        ;
    Uint32             temp                           ;

    TRC_4ENTER ("DM6437_halVlynqWrite", halObj, dspAddr, cBytes, writeBuffer) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (cBytes > 0) ;
    DBC_Require (writeBuffer != NULL) ;

    pvlynqRegs  = (DM6437_vlynqRegs *) (  halObject->vlynq.region3Addr
                                        + DM6437_PEERVLYNQREG_BASE) ;

    /* Save the original page base address */
    orgPageBase = pvlynqRegs->LOCAL_RAMO2 ;

    while (cBytes > 0) {
        pvlynqRegs->LOCAL_RAMO2 = (dspAddr + wBytes) ;
        curHostPtr = ((Char8 *) ((Uint32) writeBuffer + wBytes)) ;
        dspPtr     = (Char8 *) halObject->vlynq.region2Addr ;

        if (pvlynqRegs->LOCAL_RAMS2 >= cBytes) {
            if (cBytes != sizeof(Uint32)) {
                MEM_Copy ((unsigned char *) dspPtr,
                          (unsigned char *) curHostPtr,
                          cBytes,
                          Endianism_Default) ;
            }
            else {
                /* For 4 bytes, directly write as a Uint32 */
                temp8_1 = ((Uint8 *)curHostPtr)[0] ;
                temp8_2 = ((Uint8 *)curHostPtr)[1] ;
                temp8_3 = ((Uint8 *)curHostPtr)[2] ;
                temp8_4 = ((Uint8 *)curHostPtr)[3] ;
                temp = (Uint32) (    ((Uint32) temp8_4 << 24)
                                 |   ((Uint32) temp8_3 << 16)
                                 |   ((Uint32) temp8_2 << 8)
                                 |   ((Uint32) temp8_1)) ;
                *((Uint32*)dspPtr)      = temp ;
            }
            cBytes = 0 ;
        }
        else {
            wBytes += pvlynqRegs->LOCAL_RAMS2 ;
            if (pvlynqRegs->LOCAL_RAMS2 != sizeof(Uint32) ) {
                MEM_Copy ((unsigned char *) dspPtr,
                          (unsigned char *) curHostPtr,
                          pvlynqRegs->LOCAL_RAMS2,
                          Endianism_Default) ;
            }
            else {
                /* For 4 bytes, directly write as a Uint32 */
                temp8_1 = ((Uint8 *)curHostPtr)[0] ;
                temp8_2 = ((Uint8 *)curHostPtr)[1] ;
                temp8_3 = ((Uint8 *)curHostPtr)[2] ;
                temp8_4 = ((Uint8 *)curHostPtr)[3] ;
                temp = (Uint32) (    ((Uint32) temp8_4 << 24)
                                 |   ((Uint32) temp8_3 << 16)
                                 |   ((Uint32) temp8_2 << 8)
                                 |   ((Uint32) temp8_1)) ;
                *((Uint32*)dspPtr)      = temp ;
            }
            cBytes -= pvlynqRegs->LOCAL_RAMS2 ;
        }
    }

    /* Restore the original page base address */
    pvlynqRegs->LOCAL_RAMO2 = orgPageBase ;

    TRC_1LEAVE ("DM6437_halVlynqWrite", status) ;

    return status ;
}




#if defined (__cplusplus)
}
#endif
