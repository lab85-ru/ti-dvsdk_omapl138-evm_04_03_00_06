/** ============================================================================
 *  @file   dm6437_phy_pci.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/pci/Linux/2.6.18/
 *
 *  @desc   Linux pci driver interface file.
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


/*  ----------------------------------- OS Headers                  */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal.h>
#include <dm6437_hal_pci_boot.h>
#include <dm6437_hal_pci_intgen.h>
#include <dm6437_hal_pci_pwr.h>
#include <dm6437_hal_pci_map.h>
#include <dm6437_hal_pci_dma.h>
#include <dm6437_hal_pci_rdwr.h>
#include <dm6437_phy_pci.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_PCI_PHY

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON      TRC_SetReason (status, FID_C_ARCH_PHY_PCI, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  PCI_TI_VENDOR
 *
 *  @desc   TI Vendor ID.
 *  ============================================================================
 */
#define PCI_TI_VENDOR   0x104c

/** ============================================================================
 *  @const  PCI_TI_DEVICE
 *
 *  @desc   PCI Device ID.
 *  ============================================================================
 */
#define PCI_TI_DEVICE   0xB001


/** ============================================================================
 *  @name   DM6437HAL_pciInterface
 *
 *  @desc   Interface functions exported by the PCI subcomponent.
 *  ============================================================================
 */
HAL_Interface DM6437_pciInterface = {
                                      &DM6437_phyPciInit,
                                      &DM6437_phyPciExit,
                                      &DM6437_halPciBootCtrl,
                                      &DM6437_halPciIntCtrl,
                                      NULL,
                                      &DM6437_halPciMapCtrl,
                                      &DM6437_halPciPwrCtrl,
                                      &DM6437_halPciRead,
                                      &DM6437_halPciWrite,
                                      &DM6437_halPciReadDMA,
                                      &DM6437_halPciWriteDMA,
                                    } ;


/* ============================================================================
 *  @func   DM6437_phyEnableDevice
 *
 *  @desc   This function enables the given pci device..
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    Returns -1 on failure otherwise 0.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int32
DM6437_phyEnableDevice (IN DM6437_HalObj * halObject) ;


/* ============================================================================
 *  @func   DM6437_phyDisableDevice
 *
 *  @desc   This function disables the given pci device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Int32
DM6437_phyDisableDevice (IN DM6437_HalObj * halObject) ;


/* ============================================================================
 *  @func   DM6437_phyPciInit
 *
 *  @desc   Initializes PCI device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_phyPciInit (IN Pvoid halObj)
{
    DSP_STATUS       status    = DSP_SOK                  ;
    DM6437_HalObj *  halObject = (DM6437_HalObj *) halObj ;
    struct pci_dev * dev       = NULL                     ;
    Int32            osStatus                             ;
    Uint32           bar2Start                            ;
    Uint32           bar2Len                              ;
    Uint32 *         regVirtPtr                           ;
    Uint8            i                                    ;
    struct pci_bus * dev_bus                              ;
    Int32            dev_fn                               ;

    TRC_1ENTER ("DM6437_phyPciInit", halObj) ;

    DBC_Require (NULL != halObject) ;

/* For backward compatiblity */
#if (MAX_DSPS > 1)
    dev = pci_find_slot (halObject->pci.busNo,
                         ((halObject->pci.slotNo & 0x1f) << 3)) ;
    if (NULL == dev) {
        /* Specified PCI card was not found at the particular BUS and slot */
        status = DSP_ENOTFOUND ;
    }
    else {
        if (   (PCI_TI_DEVICE == dev->device)
            && (PCI_TI_VENDOR == dev->vendor)) {
            halObject->pci.irqId = dev->irq ;
            halObject->pci.dev = (Pvoid) dev ;
        }
        else {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
    }
#else
/* use the default DSP if single DSP is used */
    if ((dev = pci_get_device (PCI_TI_VENDOR, PCI_TI_DEVICE, dev)) != NULL) {
        halObject->pci.irqId = dev->irq ;
        halObject->pci.dev = (Pvoid) dev ;
    }
    else {
        status = DSP_ENOTFOUND ;
        SET_FAILURE_REASON ;
    }
#endif /* #if (MAX_DSPS > 1) */

    dev_bus = dev->bus ;
    dev_fn  = dev->devfn ;
    /* Patching the BAR0 size and initial address, as follows:
     * 1. Change the TRL for BAR0 and MASK register for BAR0.
     * 2. Remove the Device for Linux.
     * 3. Rescan the device and attach it Linux list of devices.
     * note: BAR2 points to CFG register.
     */
    /* MAP CFG area */
    bar2Start = pci_resource_start (dev, 2u) ;
    bar2Len   = pci_resource_len   (dev, 2u) ;
    request_mem_region (bar2Start, bar2Len, "PCIDRV") ;
    regVirtPtr = ioremap (bar2Start, bar2Len) ;

    if (regVirtPtr != NULL) {
        /* 32M for BAR0, MASK register is at 110u offset */
        *((Uint32 *) ((Uint32)regVirtPtr + 0x1A110u)) = 0xFE000008u ;
        /* Initial start address is 0x80000000,
         * TRL register is at 1C0u offset
         */
        *((Uint32 *) ((Uint32)regVirtPtr + 0x1A1C0u)) = 0x80000000u ;
    }

    /* Unmap CFG area */
    if (regVirtPtr != NULL) {
        iounmap (regVirtPtr) ;
        release_mem_region (bar2Start, bar2Len);
    }

    /* Release the dev structure */
    pci_dev_put (dev) ;

    /* Release the device */
    for (i = 0 ; i < 8 ; i++) {
        struct pci_dev * dev = pci_get_slot (dev_bus, dev_fn + i) ;
        if (dev) {
            pci_remove_bus_device (dev) ;
            pci_dev_put (dev) ;
        }
    }

    /* Rescan the device */
    pci_scan_slot (dev_bus, dev_fn) ;
    pci_bus_assign_resources (dev_bus) ;
    /* Fix the IRQ */
    dev = pci_get_slot (dev_bus, dev_fn) ;
    pci_write_config_byte(dev, PCI_INTERRUPT_LINE, halObject->pci.irqId) ;
    dev->irq = halObject->pci.irqId ;
    pci_dev_put(dev) ;

    pci_bus_add_devices (dev_bus) ;

    /* Find the device again */
    dev = NULL ;
/* For backward compatiblity */
#if (MAX_DSPS > 1)
    dev = pci_find_slot (halObject->pci.busNo,
                         ((halObject->pci.slotNo & 0x1f) << 3)) ;
    if (NULL == dev) {
        /* Specified PCI card was not found at the particular BUS and slot */
        status = DSP_ENOTFOUND ;
    }
    else {
        if (   (PCI_TI_DEVICE == dev->device)
            && (PCI_TI_VENDOR == dev->vendor)) {
            halObject->pci.dev = (Pvoid) dev ;
        }
        else {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
    }
#else
/* use the default DSP if single DSP is used */
    if ((dev = pci_get_device (PCI_TI_VENDOR, PCI_TI_DEVICE, dev)) != NULL) {
        halObject->pci.dev = (Pvoid) dev ;
    }
    else {
        status = DSP_ENOTFOUND ;
        SET_FAILURE_REASON ;
    }
#endif /* #if (MAX_DSPS > 1) */

    if (DSP_SUCCEEDED (status)) {
        osStatus = DM6437_phyEnableDevice (halObject) ;
        if (osStatus < 0) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Create the lock for DMA resource */
        halObject->lock = SYNC_SpinLockCreate () ;
    }

    TRC_1LEAVE ("DM6437_phyPciInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM6437_phyPciExit
 *
 *  @desc   Finalizes PCI device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_phyPciExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;
    DM6437_HalObj * halObject = (DM6437_HalObj *) halObj ;

    TRC_1ENTER ("DM6437_phyPciExit", halObj) ;

    DBC_Require (NULL != halObject) ;

    SYNC_SpinLockDelete (halObject->lock) ;

    DM6437_phyDisableDevice (halObject) ;

    TRC_1LEAVE ("DM6437_phyPciExit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM6437_phyEnableDevice
 *
 *  @desc   This function enables the given pci device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Int32
DM6437_phyEnableDevice (IN DM6437_HalObj * halObject)
{
    struct pci_dev * dev = NULL ;
    Int32            status = 0    ;
    Uint32           barStart [NUM_BARS]  ;
    Uint32           barLen   [NUM_BARS]  ;
    Uint32           barFlags [NUM_BARS]  ;
    Uint32           barVirt  [NUM_BARS]  ;
    Uint32           i                    ;
    Uint16           cmdVal               ;

    DBC_Require (NULL != halObject) ;

    dev = (struct pci_dev *) halObject->pci.dev ;

    /* Enable the deivce. */
    status = pci_enable_device (dev) ;
    if (status >= 0) {
       halObject->pci.irqId = dev->irq ;

        /* read base address region. */
        for (i = 0; i < NUM_BARS && (status != -1); i++) {
            barStart [i] = pci_resource_start (dev, i) ;
            barLen   [i] = pci_resource_len   (dev, i) ;
            barFlags [i] = pci_resource_flags (dev, i) ;

            if (barFlags [i] & IORESOURCE_MEM) {
                /* Map the memory region. */
                request_mem_region (barStart [i], barLen [i], "DSPLINK");
            }
            else {
                /* Map the memory region. */
                request_region (barStart [i], barLen [i], "DSPLINK");
            }

            if (barStart [i] > 0) {
                barVirt [i] = (Uint32) ioremap (barStart [i], barLen [i]) ;
                halObject->pci.barPhys [i] = barStart [i] ;
                halObject->pci.barVirt [i] = barVirt  [i] ;
                halObject->pci.barLen  [i] = barLen   [i] ;
            }
            else {
                status = -1 ;
            }
        }
    }


    if (status >= 0) {
        /* set the our PCI dev structure in linux device structure. */
        pci_set_drvdata (dev, (void *) halObject) ;

        /* set the DMA mask */
        pci_set_dma_mask (dev, 0xfffffff0ULL) ;

        status = pci_set_mwi (dev) ;
        /*
         * set the desired PCI dev to be master, this internally sets the
         * latency timer.
         */
        pci_set_master (dev) ;
        pci_write_config_byte(dev, PCI_LATENCY_TIMER, 0x80);

        pci_read_config_word (dev, PCI_COMMAND, (u16 *) &cmdVal) ;
        /* and set the master bit in command register. */
        cmdVal |=   PCI_COMMAND_MEMORY
                  | PCI_COMMAND_MASTER
                  | PCI_COMMAND_SERR  ;
        /* and clear the interrupt disable bit in command register. */
#if defined (PCI_COMMAND_INTX_DISABLE)
        cmdVal &=   ~PCI_COMMAND_INTX_DISABLE ;
#else
        cmdVal &=   ~0x400 ;
#endif
        pci_write_config_word (dev, PCI_COMMAND, cmdVal) ;
    }

    return status ;
}


/* ============================================================================
 *  @func   DM6437_phyDisableDevice
 *
 *  @desc   This function disables the given pci device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Int32
DM6437_phyDisableDevice (IN DM6437_HalObj * halObject)
{
    struct pci_dev * dev = NULL ;
    Int32            status = 0    ;
    Uint32           i             ;
    Uint16           cmdVal               ;

    DBC_Require (NULL != halObject) ;

    dev = (struct pci_dev *) halObject->pci.dev ;

    /* Unmap baseMem region & release the memory region. */
    for (i = 0; i < NUM_BARS; i++) {
        iounmap((Pvoid) halObject->pci.barVirt [i]) ;
        if (pci_resource_flags (dev, i) & IORESOURCE_MEM) {
            /* Map the memory region. */
            release_mem_region (halObject->pci.barPhys [i],
                                halObject->pci.barLen [i]) ;
        }
        else {
            /* Map the memory region. */
            release_region (halObject->pci.barPhys [i],
                            halObject->pci.barLen [i]) ;
        }
    }

    if (status >= 0) {
        /* set the our PCI dev structure in linux device structure. */
        pci_set_drvdata (dev, NULL) ;

        pci_read_config_word (dev, PCI_COMMAND, &cmdVal) ;
        cmdVal  &= ~(  PCI_COMMAND_MEMORY
                     | PCI_COMMAND_MASTER
                     | PCI_COMMAND_SERR) ;
        pci_write_config_word (dev, PCI_COMMAND, cmdVal) ;
        pci_disable_device (dev) ;
    }

    return status ;
}


#if defined (__cplusplus)

}
#endif
