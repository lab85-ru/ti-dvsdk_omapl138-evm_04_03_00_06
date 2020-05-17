/** ============================================================================
 *  @file   _dsplink.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/drv/
 *
 *  @desc   Defines internal constants, data types and structures used by
 *          the DRV component for DSP/BIOS(TM) LINK.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (_DSPLINK_H)
#define _DSPLINK_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <shm_drv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  DRV_HANDSHAKE_BASE
 *
 *  @desc   Indicates the base value for handshake used by the DRV component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_BASE        (Uint16) 0x0

/** ============================================================================
 *  @const  DRV_HANDSHAKE_DRV
 *
 *  @desc   Indicates handshake for the DRV component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_DRV         0x0001

/** ============================================================================
 *  @const  DRV_HANDSHAKE_IPS
 *
 *  @desc   Indicates handshake for the IPS component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_IPS         0x0002

#if defined (POOL_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_POOL
 *
 *  @desc   Indicates handshake for the POOL component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_POOL        0x0004
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MPCS
 *
 *  @desc   Indicates handshake for the MPCS component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MPCS        0x0008
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MPLIST
 *
 *  @desc   Indicates handshake for the MPLIST component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MPLIST      0x0010
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_MQT
 *
 *  @desc   Indicates handshake for the MQT component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_MQT         0x0020
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_DATA
 *
 *  @desc   Indicates handshake for the DATA component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_DATA        0x0040
#endif /* if defined (CHNL_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_NOTIFY
 *
 *  @desc   Indicates handshake for the NOTIFY component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_NOTIFY      0x0080
#endif /* if defined (NOTIFY_COMPONENT) */

#if defined (RINGIO_COMPONENT)
/** ============================================================================
 *  @const  DRV_HANDSHAKE_RINGIO
 *
 *  @desc   Indicates handshake for the RingIO component.
 *  ============================================================================
 */
#define DRV_HANDSHAKE_RINGIO      0x0100
#endif /* if defined (RINGIO_COMPONENT) */


/** ============================================================================
 *  @name   DRV_Config
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
typedef struct DRV_Config_tag {
    Uint32       numIpsEntries ;
    Uint32       numPools ;
    Uint32       numDataDrivers ;
    Uint32       cpuFreq ;
} DRV_Config ;

/** ============================================================================
 *  @name   DRV_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          DRV component.
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
 *  @field  ipsAddr
 *              DSP Shared memory address of the IPS components.
 *  @field  ipsDspInitDone
 *              Indicates whether DSP-side initialization of the IPS component
 *              was successful.
 *  @field  poolAddr
 *              DSP Shared memory address of the POOL components.
 *  @field  poolConfigured
 *              Indicates whether POOL component has been configured on GPP-side
 *  @field  poolDspInitDone
 *              Indicates whether DSP-side initialization of the POOL component
 *              was successful.
 *  @field  mpcsAddr
 *              DSP Shared memory address of the MPCS component.
 *  @field  mpcsConfigured
 *              Indicates whether MPCS component has been configured on GPP-side
 *  @field  mpcsDspInitDone
 *              Indicates whether DSP-side initialization of the MPCS component
 *              was successful.
 *  @field  mplistAddr
 *              DSP Shared memory address of the MPLIST component.
 *  @field  mplistConfigured
 *              Indicates whether MPLIST component has been configured on
 *              GPP-side.
 *  @field  mplistDspInitDone
 *              Indicates whether DSP-side initialization of the MPLIST
 *              component was successful.
 *  @field  mqtAddr
 *              DSP Shared memory address of the MQT component.
 *  @field  mqtConfigured
 *              Indicates whether MQT component has been configured on GPP-side.
 *  @field  mqtDspInitDone
 *              Indicates whether DSP-side initialization of the MQT component
 *              was successful.
 *  @field  dataAddr
 *              DSP Shared memory address of the DATA component.
 *  @field  dataConfigured
 *              Indicates whether DATA component has been configured on GPP-side
 *  @field  dataDspInitDone
 *              Indicates whether DSP-side initialization of the DATA component
 *              was successful.
 *  @field  ringIoAddr
 *              DSP Shared memory address of the RINGIO component.
 *  @field  ringIoConfigured
 *              Indicates whether RINGIO component has been configured on
 *              GPP-side.
 *  @field  ringIoDspInitDone
 *              Indicates whether DSP-side initialization of the RINGIO
 *              component was successful.
 *  @field  config
 *              Configuration structure for the DRV instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DRV_Ctrl_tag {
    Char         version [DSP_MAX_STRLEN] ;
    Uint32       procId ;
    Uint32       linkAddr ;
    Uint32       drvDspInitDone ;
    Uint32       ipsAddr ;
    Uint32       ipsDspInitDone ;
    Uint32       poolAddr ;
    Uint32       poolConfigured ;
    Uint32       poolDspInitDone ;
    Uint32       mpcsAddr ;
    Uint32       mpcsConfigured ;
    Uint32       mpcsDspInitDone ;
    Uint32       mplistAddr ;
    Uint32       mplistConfigured ;
    Uint32       mplistDspInitDone ;
    Uint32       mqtAddr ;
    Uint32       mqtConfigured ;
    Uint32       mqtDspInitDone ;
    Uint32       dataAddr ;
    Uint32       dataConfigured ;
    Uint32       dataDspInitDone ;
    Uint32       ringIoAddr ;
    Uint32       ringIoConfigured ;
    Uint32       ringIoDspInitDone ;
    DRV_Config   config ;
    ADD_PADDING (padding, DRV_PADDING)
} DRV_Ctrl ;


/** ============================================================================
 *  @func   _DSPLINK_init
 *
 *  @desc   This function initializes DSP/BIOS LINK. Tihs is the internal
 *          implementation for the function to initialize DSPLINK.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Void
_DSPLINK_init (Void) ;


/** ============================================================================
 *  @func   DSPLINK_handshake
 *
 *  @desc   Performs handshaking with the GPP to synchronize the drivers on both
 *          processors.
 *
 *  @arg    procId
 *              Processor Identifier with which the driver is shared.
 *  @arg    dspHandshake
 *              DSP handshake value to be sent to the GPP.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
inline Void DSPLINK_handshake (IN Uint32 procId, IN Uint16 dspHandshake)
{
    /* Handshake with the GPP */
    SHMDRV_handshake (procId, dspHandshake) ;
}



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (_DSPLINK_H) */
