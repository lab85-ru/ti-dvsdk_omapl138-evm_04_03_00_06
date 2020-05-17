/** ============================================================================
 *  @file   dm6437_hal_pci_rdwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/pci/
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

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <sync.h>
#include <mem_os.h>
#include <mem.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_pci.h>
#include <dm6437_hal_pci_rdwr.h>


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
 *  @func   DM6437_halPciRead
 *
 *  @desc   Reads a buffer from dsp memory (for now this is done as bytes)
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halPciRead (IN  Pvoid          halObj,
                   IN  Uint32         dspAddr,
                   IN  Uint32         cBytes,
                   OUT Char8 *        readBuffer)
{
    DSP_STATUS        status            = DSP_SOK    ;
    DM6437_pciRegs *  pciRegs           = NULL       ;
    DM6437_HalObj *   halObject         = (DM6437_HalObj *) halObj ;
    Char8 *           curHostPtr        = readBuffer ;
    Char8 *           dspPtr            = NULL       ;
    Uint32            rBytes            = 0          ;
    Uint32            availBytes        = 0          ;
    Uint32            orgPageBase                    ;
    Uint32            irqFlags                       ;

    TRC_4ENTER ("DM6437_halPciRead", halObj, dspAddr, cBytes, readBuffer) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (cBytes > 0) ;
    DBC_Require (readBuffer != NULL) ;

    pciRegs  = (DM6437_pciRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                                 + DM6437_PCIREG_BASE) ;

    irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        /* Save the original page base address */
        orgPageBase = pciRegs->PCIBAR4TRL ;

        while (cBytes > 0) {
            pciRegs->PCIBAR4TRL = (dspAddr + rBytes) & (pciRegs->PCIBAR4MSK) ;
            curHostPtr = ((Char8 *) ((Uint32) readBuffer + rBytes)) ;
            dspPtr     = (Char8 *) (  halObject->pci.barVirt [RWMEM_BAR_NO]
                                 + ((dspAddr + rBytes) - pciRegs->PCIBAR4TRL)) ;
            /* Available bytes in the window is calculated as follows:
            * windowsize - (current pointer in the window -
            */
            availBytes = (  (halObject->pci.barLen [RWMEM_BAR_NO])
                         - ((dspAddr + rBytes) - pciRegs->PCIBAR4TRL)) ;

            if (availBytes >= cBytes) {
                MEM_Copy ((unsigned char *) curHostPtr,
                          (unsigned char *) dspPtr,
                          cBytes,
                          Endianism_Default) ;
                cBytes = 0 ;
            }
            else {
                rBytes += availBytes ;
                MEM_Copy ((unsigned char *)curHostPtr,
                          (unsigned char *) dspPtr,
                          availBytes,
                          Endianism_Default) ;
                cBytes -= availBytes ;
            }
        }

        /* Restore the original page base address */
        pciRegs->PCIBAR4TRL = orgPageBase ;

    SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;

    TRC_1LEAVE ("DM6437_halPciRead", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6437_halPciWrite
 *
 *  @desc   Writes a buffer to dsp memory (for now this is done as bytes)
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halPciWrite (IN  Pvoid          halObj,
                    IN  Uint32         dspAddr,
                    IN  Uint32         cBytes,
                    IN  Char8 *        writeBuffer)
{
    DSP_STATUS        status           = DSP_SOK     ;
    DM6437_pciRegs *  pciRegs          = NULL        ;
    DM6437_HalObj *    halObject        = (DM6437_HalObj *) halObj ;
    Char8 *           curHostPtr       = writeBuffer ;
    Char8 *           dspPtr           = NULL        ;
    Uint32            wBytes           = 0           ;
    Uint32            availBytes       = 0           ;
    Uint32            orgPageBase                    ;
    Uint32            irqFlags                       ;
    Uint8             temp8_1                        ;
    Uint8             temp8_2                        ;
    Uint8             temp8_3                        ;
    Uint8             temp8_4                        ;
    Uint32            temp                           ;

    TRC_4ENTER ("DM6437_halPciWrite", halObj, dspAddr, cBytes, writeBuffer) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (cBytes > 0) ;
    DBC_Require (writeBuffer != NULL) ;

    pciRegs  = (DM6437_pciRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                       + DM6437_PCIREG_BASE) ;

    irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        /* Save the original page base address */
        orgPageBase = pciRegs->PCIBAR4TRL ;

        while (cBytes > 0) {
            pciRegs->PCIBAR4TRL = (dspAddr + wBytes) & (pciRegs->PCIBAR4MSK) ;
            curHostPtr = ((Char8 *) (((Uint32) writeBuffer) + wBytes)) ;
            dspPtr     = (Char8 *) (  halObject->pci.barVirt [RWMEM_BAR_NO]
                                    + ((dspAddr + wBytes) - pciRegs->PCIBAR4TRL)) ;
            /* Available bytes in the window is calculated as follows:
             * windowsize - (current pointer in the window -
             */
            availBytes = (  (halObject->pci.barLen [RWMEM_BAR_NO])
                        - ((dspAddr + wBytes) - pciRegs->PCIBAR4TRL)) ;

            if (availBytes >= cBytes) {
                 if (cBytes != sizeof(Uint32)) {
                    MEM_Copy ((unsigned char *)dspPtr,
                         (unsigned char *)curHostPtr,
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
                wBytes += availBytes ;
                if (availBytes != sizeof(Uint32) ) {
                     MEM_Copy ((unsigned char *) dspPtr,
                          (unsigned char *) curHostPtr,
                          availBytes,
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
                cBytes -= availBytes ;
            }
    }

    /* Restore the original page base address */
    pciRegs->PCIBAR4TRL = orgPageBase ;

    SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;

    TRC_1LEAVE ("DM6437_halPciWrite", status) ;

    return status ;
}

#if defined (__cplusplus)
}
#endif
