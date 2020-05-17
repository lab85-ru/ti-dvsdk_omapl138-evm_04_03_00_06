/** ============================================================================
 *  @file   ldrv_drv.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DRV/
 *
 *  @desc   Defines interfaces exposed by LDRV_DRV subcomponent. These services
 *          are used by LDRV_PROC for performing driver initialization and
 *          synchronization. It uses services from (LDRV_)DSP subcomponent
 *          for carrying out the tasks.
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


#if !defined (LDRV_DRV_H)
#define LDRV_DRV_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_linkdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  DRV_HANDSHAKE_BASE
 *
 *  @desc   Indicates the base value for handshake used by the DRV component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_BASE        (Uint16) 0x0u

/** ============================================================================
 *  @const  DRV_HANDSHAKE_DRV
 *
 *  @desc   Indicates handshake for the DRV component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_DRV         0x0001u

/** ============================================================================
 *  @const  DRV_HANDSHAKE_IPS
 *
 *  @desc   Indicates handshake for the IPS component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_IPS         0x0002u

/** ============================================================================
 *  @const  NUM_CHARS_VERSION
 *
 *  @desc   Indicates number of characters to be checked for version matching
 *          This will check only 1.6\0
 *  ============================================================================
 */
#define NUM_CHARS_VERSION       4

#if defined (POOL_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_POOL
 *
 *  @desc   Indicates handshake for the POOL component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_POOL        0x0004u
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MPCS
 *
 *  @desc   Indicates handshake for the MPCS component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MPCS        0x0008u
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MPLIST
 *
 *  @desc   Indicates handshake for the MPLIST component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MPLIST      0x0010u
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MQT
 *
 *  @desc   Indicates handshake for the MQT component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MQT         0x0020u
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_DATA
 *
 *  @desc   Indicates handshake for the DATA component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_DATA        0x0040u
#endif /* if defined (CHNL_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_NOTIFY
 *
 *  @desc   Indicates handshake for the NOTIFY component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_NOTIFY      0x0080u
#endif /* if defined (NOTIFY_COMPONENT) */

#if defined (RINGIO_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_RINGIO
 *
 *  @desc   Indicates handshake for the RingIO component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_RINGIO      0x0100u
#endif /* if defined (RINGIO_COMPONENT) */


/** ============================================================================
 *  @name   LDRV_DRV_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the DRV components on the GPP and DSP-sides.
 *
 *  @field  numIpsEntries
 *              Number of IPS table entries for this link driver.
 *  @field  numPools
 *              Number of POOLs within the pool table for this link driver.
 *  @field  numDataDrivers
 *              Number of data drivers used with this DSP.
 *  @field  cpuFreq
 *              The frequency at which the DSP is running (in KHz).
 *  ============================================================================
 */
typedef struct LDRV_DRV_Config_tag {
    Uint32       numIpsEntries ;
    Uint32       numPools ;
    Uint32       numDataDrivers ;
    Uint32       cpuFreq ;
} LDRV_DRV_Config ;

/** ============================================================================
 *  @name   LDRV_DRV_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          LDRV_DRV component.
 *
 *  @field  version
 *              Version string for DSP/BIOS LINK.
 *  @field  procId
 *              Processor Identifier.
 *  @field  linkAddr
 *              DSP Shared memory address of the link to the DSP.
 *  @field  drvDspInitDone
 *              Indicates whether DSP-side initialization of the DRV component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  ipsAddr
 *              DSP Shared memory address of the LDRV IPS component.
 *  @field  ipsDspInitDone
 *              Indicates whether DSP-side initialization of the IPS component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  poolAddr
 *              DSP Shared memory address of the LDRV POOL component.
 *  @field  poolConfigured
 *              Indicates whether POOL component has been configured on GPP-side
 *  @field  poolDspInitDone
 *              Indicates whether DSP-side initialization of the POOL component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  mpcsAddr
 *              DSP Shared memory address of the LDRV MPCS component.
 *  @field  mpcsConfigured
 *              Indicates whether MPCS component has been configured on GPP-side
 *  @field  mpcsDspInitDone
 *              Indicates whether DSP-side initialization of the MPCS component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  mplistAddr
 *              DSP Shared memory address of the LDRV MPLIST component.
 *  @field  mplistConfigured
 *              Indicates whether MPLIST component has been configured on
 *              GPP-side.
 *  @field  mplistDspInitDone
 *              Indicates whether DSP-side initialization of the MPLIST
 *              component was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  mqtAddr
 *              DSP Shared memory address of the LDRV MQT component.
 *  @field  mqtConfigured
 *              Indicates whether MQT component has been configured on GPP-side.
 *  @field  mqtDspInitDone
 *              Indicates whether DSP-side initialization of the MQT component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  dataAddr
 *              DSP Shared memory address of the LDRV DATA component.
 *  @field  dataConfigured
 *              Indicates whether DATA component has been configured on GPP-side
 *  @field  dataDspInitDone
 *              Indicates whether DSP-side initialization of the DATA component
 *              was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  ringIoAddr
 *              DSP Shared memory address of the LDRV RINGIO component.
 *  @field  ringIoConfigured
 *              Indicates whether RINGIO component has been configured on
 *              GPP-side.
 *  @field  ringIoDspInitDone
 *              Indicates whether DSP-side initialization of the RINGIO
 *              component was successful.
 *              0              -> success
 *              Positive value -> DSP-side failure code.
 *              (Uint32) -1    -> DSP-side component was not initialized.
 *  @field  config
 *              Configuration structure for the DRV instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct LDRV_DRV_Ctrl_tag {
    Char8           version [DSP_MAX_STRLEN] ;
    Uint32          procId ;
    Uint32          linkAddr ;
    Uint32          drvDspInitDone ;
    Uint32          ipsAddr ;
    Uint32          ipsDspInitDone ;
    Uint32          poolAddr ;
    Uint32          poolConfigured ;
    Uint32          poolDspInitDone ;
    Uint32          mpcsAddr ;
    Uint32          mpcsConfigured ;
    Uint32          mpcsDspInitDone ;
    Uint32          mplistAddr ;
    Uint32          mplistConfigured ;
    Uint32          mplistDspInitDone ;
    Uint32          mqtAddr ;
    Uint32          mqtConfigured ;
    Uint32          mqtDspInitDone ;
    Uint32          dataAddr ;
    Uint32          dataConfigured ;
    Uint32          dataDspInitDone ;
    Uint32          ringIoAddr ;
    Uint32          ringIoConfigured ;
    Uint32          ringIoDspInitDone ;
    LDRV_DRV_Config config ;
    ADD_PADDING (padding, LDRV_DRV_PADDING)
} LDRV_DRV_Ctrl ;


/** ============================================================================
 *  @func   LDRV_DRV_moduleInit
 *
 *  @desc   This function initializes the LDRV_DRV module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DRV_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_DRV_moduleExit
 *
 *  @desc   This function finalizes the LDRV_DRV module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DRV_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_DRV_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DRV_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_init (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_DRV_exit
 *
 *  @desc   This function finalizes the LDRV DRV module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DRV_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_DRV_handshake
 *
 *  @desc   Does the necessary handshake (if required) between the drivers
 *          on the GPP & DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    hshkCtrl
 *              Handshake control action to be executed.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_handshake (IN ProcessorId dspId, IN DRV_Handshake hshkCtrl) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_DRV_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    None.
 *
 *  @enter  dspId shall be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_DRV_debug (IN  ProcessorId  dspId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_DRV_H) */
