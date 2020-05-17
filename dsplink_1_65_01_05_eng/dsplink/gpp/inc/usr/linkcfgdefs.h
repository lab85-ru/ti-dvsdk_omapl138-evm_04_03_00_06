/** ============================================================================
 *  @file   linkcfgdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines constants and interfaces for configuration of DSPLINK.
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


#if !defined (LINKCFGDEFS_H)
#define LINKCFGDEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <linkcfgdefs_os.h>


#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LINKCFG_Gpp
 *
 *  @desc   This structure defines the configuration structure for the GPP.
 *
 *  @field  name
 *              Name of GPP Processor.
 *  @field  maxMsgqs
 *              Maximum MSGQs that can be opened on the GPP.
 *  @field  maxChnlQueue
 *              Maximum Queue Length for all channels created on the GPP.
 *  @field  poolTableId
 *              POOL table ID to be used for intra-GPP communication. A value of
 *              -1 indicates that no POOL is required by the GPP.
 *  @field  numPools
 *              Number of POOLs within the pool table for the GPP.
 *  @field  gppOsObject
 *              Pointer to the GPP OS-specific configuration object.
 *  ============================================================================
 */
typedef struct LINKCFG_Gpp_tag {
    Char8              name [DSP_MAX_STRLEN] ;
    Uint32             maxMsgqs ;
    Uint32             maxChnlQueue ;
    Uint32             poolTableId ;
    Uint32             numPools ;
    LINKCFG_GppOs *    gppOsObject ;
} LINKCFG_Gpp ;


/** ============================================================================
 *  @name   DSP_BootMode
 *
 *  @desc   This structure defines the configuration structure for the GPP.
 *
 *  @field  DSP_BootMode_Boot_NoPwr
 *              This is backward compatible with the default false i.e.
 *              DSPLINK_BOOT_MODE. Wherein DSPLink does not do power management
 *              of DSP
 *              PROC_attach will put DSP in local reset. It will not power up
 *              the DSP.
 *              PROC_start will set entry point for DSP i.e. c_int00 and release
 *              DSP from reset.
 *              PROC_stop will put DSP in local reset.
 *              PROC_detach will not power down the DSP.
 *
 *  @field  DSP_BootMode_Boot_Pwr
 *              This is backward compatible with the default true i.e.
 *              DSPLINK_BOOT_MODE. Wherein DSPLink does power management of DSP
 *              PROC_attach will put DSP in local reset. It will power up the
 *              DSP.
 *              PROC_start will set entry point for DSP i.e. c_int00 and release
 *              DSP from reset.
 *              PROC_stop will put DSP in local reset.
 *              PROC_detach will power down the DSP.
 *
 *  @field  DSP_BootMode_NoLoad_NoPwr
 *              This is added to support DSPLINK_NOLOAD_MODE.
 *              PROC_attach will put DSP in local reset. It will not power up
 *              the DSP.
 *              PROC_start will set entry point for DSP i.e. c_int00 and release
 *              DSP from reset
 *              PROC_stop will put DSP in local reset.
 *              PROC_detach will not power down the DSP.
 *
 *  @field  DSP_BootMode_NoLoad_Pwr
 *              This is added to support DSPLINK_NOLOAD_MODE.
 *              PROC_attach will put DSP in local reset. It will power up
 *              the DSP.
 *              PROC_start will set entry point for DSP i.e. c_int00 and release
 *              DSP from reset
 *              PROC_stop will put DSP in local reset.
 *              PROC_detach will power down the DSP.
 *
 *  @field  DSP_BootMode_NoBoot
 *              This is added to support DSPLINK_NOBOOT_MODE
 *              PROC_attach will not put DSP in local reset. It will not power
 *              up the DSP.
 *              PROC_start will not set entry point for DSP i.e. c_int00 and
 *              not release DSP from reset
 *              PROC_stop will not put DSP in local reset.
 *              PROC_detach will not power down the DSP.
 *
 *  ============================================================================
 */
typedef enum {
    DSP_BootMode_Boot_NoPwr = 0x0,
    DSP_BootMode_Boot_Pwr,
    DSP_BootMode_NoLoad_NoPwr,
    DSP_BootMode_NoLoad_Pwr,
    DSP_BootMode_NoBoot
} DSP_BootMode ;

/** ============================================================================
 *  @name   Endianism
 *
 *  @desc   Enumeration of data endianism.
 *
 *  @field  Endianism_Default
 *              Default endianism - no conversion required.
 *  @field  Endianism_Big
 *              Big endian.
 *  @field  Endianism_Little
 *              Little endian.
 *  ============================================================================
 */
typedef enum {
    Endianism_Default = 1u,
    Endianism_Big     = 2u,
    Endianism_Little  = 3u
} Endianism ;

/** ============================================================================
 *  @name   DspArch
 *
 *  @desc   Enumerates the various architectures of DSP supported by
 *          DSP/BIOS LINK.
 *
 *  @field  DspArch_Unknown
 *              It indicates that the architecture is not supported.
 *  @field  DspArch_C55x
 *              It indicates that the architecture is C55x.
 *  @field  DspArch_C64x
 *              It indicates that the architecture is C64x.
 *  @field  DspArch_C64x_Bios5
 *              It indicates that the architecture is C64x and the
 *              DSP OS version is DSP/BIOS 5.xx.
 *  @field  DspArch_C64x_Bios6
 *              It indicates that the architecture is C64x and the
 *              DSP OS version is DSP/BIOS 6.xx.
 *  ============================================================================
 */
typedef enum {
    DspArch_Unknown    = 0u,
    DspArch_C55x       = 1u,
    DspArch_C64x       = 2u,
    DspArch_C64x_Bios5 = 3u,
    DspArch_C64x_Bios6 = 4u
} DspArch ;

/** ============================================================================
 *  @name   LINKCFG_Dsp
 *
 *  @desc   This structure defines the configuration structure for the DSP.
 *
 *  @field  name
 *              Name of DSP processor.
 *  @field  dspArch
 *              Architecture of the DSP.
 *  @field  loaderName
 *              Name of loader to be used for loading the DSP executable.
 *  @field  autoStart
 *              AutoStart flag indicating whether a default DSP image should be
 *              loaded on startup.
 *              Currently not supported.
 *  @field  execName
 *              Name of executable to load in case autostart is used.
 *  @field  doDspCtrl
 *              Indicates whether DSP/BIOS LINK should do the dsp control for
 *              the DSP. This depends on the DSPLink boot mode chosen.
 *  @field  resumeAddr
 *              The resume address after hibernating.
 *  @field  resetVector
 *              Address of reset vector of DSP.
 *  @field  resetCodeSize
 *              Size of code at DSP Reset Vector.
 *  @field  maduSize
 *              Minimum addressable unit on the DSP.
 *  @field  cpuFreq
 *              The frequency at which the DSP is running (in KHz).
 *              Specify -1 if the cpuFreq is not to be set from GPP-side and the
 *              default DSP/BIOS setting is to be used.
 *  @field  endian
 *              Endianism info of DSP.
 *  @field  wordSwap
 *              Indicates whether words need to be swapped while writing into
 *              the memory for the DSP.
 *  @field  memTableId
 *              Table number of the MEM entries for this DSP.
 *  @field  memEntries
 *              Number of entries in the MEM table.
 *  @field  linkDrvId
 *              Link Driver table identifier for this DSP.
 *  @field  arg1
 *              Platform specific argument.
 *  @field  arg2
 *              Platform specific argument.
 *  @field  arg3
 *              Platform specific argument.
 *  @field  arg4
 *              Platform specific argument.
 *  @field  arg5
 *              Platform specific argument.
 *  ============================================================================
 */
typedef struct LINKCFG_Dsp_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   dspArch ;
    Char8                    loaderName [DSP_MAX_STRLEN] ;
    Bool                     autoStart ;
    Char8                    execName [DSP_MAX_STRLEN] ;
    DSP_BootMode             doDspCtrl ;
    Uint32                   resumeAddr ;
    Uint32                   resetVector ;
    Uint32                   resetCodeSize ;
    Uint32                   maduSize ;
    Uint32                   cpuFreq ;
    Uint32                   endian ;
    Uint32                   wordSwap ;
    Uint32                   memTableId ;
    Uint32                   memEntries ;
    Uint32                   linkDrvId ;
    Uint32                   arg1 ;
    Uint32                   arg2 ;
    Uint32                   arg3 ;
    Uint32                   arg4 ;
    Uint32                   arg5 ;
} LINKCFG_Dsp ;


/** ============================================================================
 *  @name   LINKCFG_MemEntry
 *
 *  @desc   This structure defines an entry in the MEM table.
 *
 *  @field  entry
 *              Entry number in the memory table.
 *  @field  name
 *              Name identifying the memory region.
 *  @field  physAddr
 *              Physical address of the memory region.
 *  @field  dspVirtAddr
 *              DSP virtual address of the memory region.
 *  @field  gppVirtAddr
 *              GPP virtual address of the memory region. If specified as -1,
 *              the GPP virtual address is assumed to be invalid, and shall be
 *              set internally within the DSPLINK driver.
 *  @field  size
 *              Size of the memory region.
 *  @field  shared
 *              Flag indicating whether the memory region is shared between GPP
 *              and DSP.
 *  @field  syncd
 *              Flag indicating whether the memory region is synchonized
 *              between GPP and DSP.
 *  ============================================================================
 */
typedef struct LINKCFG_MemEntry_tag {
    Uint32                   entry ;
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   physAddr ;
    Uint32                   dspVirtAddr ;
    Uint32                   gppVirtAddr ;
    Uint32                   size ;
    Bool                     shared ;
    Bool                     syncd  ;
} LINKCFG_MemEntry ;


/** ============================================================================
 *  @name   LINKCFG_LinkDrv
 *
 *  @desc   This structure defines the configuration information for the
 *          physical link driver.
 *
 *  @field  name
 *              Name of the physical link driver.
 *  @field  hshkPollCount
 *              Poll count to be used for the handshake between GPP and DSP. The
 *              GPP spins in a loop for number of iterations equal to the
 *              handshake poll count or till the DSP completes its handshake.
 *              A value of -1 indicates infinite wait.
 *  @field  memEntry
 *              MEM Entry for the memory area used by this physical link.
 *              This field is optional and can be specified as -1 in case
 *              a MEM entry is not required.
 *  @field  ipsTableId
 *              IPS table ID.
 *  @field  numIpsEntries
 *              Number of IPS table entries for this link driver.
 *  @field  poolTableId
 *              POOL table ID.
 *  @field  numPools
 *              Number of POOLs within the pool table for this link driver.
 *  @field  dataTableId
 *              Table number of the data driver(s) used with this DSP.
 *  @field  numDataDrivers
 *              Number of data drivers used with this DSP.
 *  @field  mqtId
 *              The ID of the MQT which is to be used for this DSP.
 *  @field  ringIoTableId
 *              Table number of the RINGIO object(s) for this DSP.
 *  @field  mplistTableId
 *              Table number of the MPLIST object(s) for this DSP.
 *  @field  mpcsTableId
 *              Table number of the MPCS object(s) for this DSP.
 *  ============================================================================
 */
typedef struct CFG_LinkDrv_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   hshkPollCount ;
    Uint32                   memEntry ;
    Uint32                   ipsTableId ;
    Uint32                   numIpsEntries ;
    Uint32                   poolTableId ;
    Uint32                   numPools ;
    Uint32                   dataTableId ;
    Uint32                   numDataDrivers ;
    Uint32                   mqtId ;
    Uint32                   ringIoTableId ;
    Uint32                   mplistTableId ;
    Uint32                   mpcsTableId ;
} LINKCFG_LinkDrv ;


/** ============================================================================
 *  @name   LINKCFG_Ips
 *
 *  @desc   This structure defines the configuration information for the
 *          Inter-processor Signaling Component.
 *
 *  @field  name
 *              Name of the IPS component.
 *  @field  numIpsEvents
 *              Number of events supported by the IPS.
 *  @field  memEntry
 *              MEM entry for the memory area used for this IPS component.
 *              This field is optional and can be specified as -1 in case
 *              a MEM entry is not required.
 *  @field  gppIntId
 *              Interrupt Number to used by the IPS on GPP-side.
 *  @field  dspIntId
 *              Interrupt Number to used by the IPS on DSP-side.
 *  @field  dspIntVectorId
 *              Interrupt Vector Number to used by the IPS on DSP-side.
 *  @field  arg1
 *              First IPS-specific optional argument.
 *  @field  arg2
 *              Second IPS-specific optional argument.
 *  ============================================================================
 */
typedef struct LINKCFG_Ips_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   numIpsEvents ;
    Uint32                   memEntry ;
    Uint32                   gppIntId ;
    Uint32                   dspIntId ;
    Uint32                   dspIntVectorId ;
    Uint32                   arg1 ;
    Uint32                   arg2 ;
} LINKCFG_Ips ;


/** ============================================================================
 *  @name   LINKCFG_Pool
 *
 *  @desc   This structure defines the configuration information for a buffer
 *          pool, from which buffers for use with DSPLINK can be allocated
 *          through the POOL interface(s).
 *
 *  @field  name
 *              Name of the pool.
 *  @field  memEntry
 *              MEM Entry for the memory area used by this buffer pool.
 *              This field is optional and can be specified as -1 in case
 *              a MEM entry is not required.
 *  @field  poolSize
 *              Size of the buffer pool.
 *  @field  ipsId
 *              ID of the IPS used (if any). A value of -1 indicates that no IPS
 *              is required by the pool.
 *  @field  ipsEventNo
 *              IPS Event number associated with POOL (if any).  A value of -1
 *              indicates that no IPS is required by the pool.
 *  @field  poolMemEntry
 *              Pool memory region section ID.
 *  @field  arg1
 *              First optional pool-specific argument.
 *  @field  arg2
 *              Second optional pool-specific argument.
 *  ============================================================================
 */
typedef struct LINKCFG_Pool_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   memEntry ;
    Uint32                   poolSize ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
    Uint32                   poolMemEntry ;
    Uint32                   arg1 ;
    Uint32                   arg2 ;
} LINKCFG_Pool ;


/** ============================================================================
 *  @name   LINKCFG_DataDrv
 *
 *  @desc   This structure defines the configuration structure for the data
 *          streaming driver using the CHNL component.
 *
 *  @field  name
 *              Name of the data transfer driver.
 *  @field  baseChnlId
 *              Base channel ID for this data driver.
 *  @field  numChannels
 *              Number of logical channels for this data driver.
 *  @field  maxBufSize
 *              Maximum size of data buffer supported by this data driver. If
 *              no limit is imposed by the driver, a value of -1 can be
 *              specified.
 *  @field  memEntry
 *              MEM entry for the memory area for data streaming driver.
 *              This field is optional and can be specified as -1 in case
 *              a MEM entry is not required.
 *  @field  poolId
 *              Identifier for the pool from where buffers are allocated.
 *  @field  queuePerChnl
 *              Buffer Queue length on each channel supported by the data
 *              driver.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the data driver.
 *  @field  ipsEventNo
 *              IPS Event number associated with data driver (if any). A value
 *              of -1 indicates that no IPS is required by the data driver.
 *  @field  arg1
 *              First optional data driver specific argument.
 *              The significance of this argument is specific to a data driver.
 *  @field  arg2
 *              Second optional data driver specific argument 2.
 *              The significance of this argument is specific to a data driver.
 *  ============================================================================
 */
typedef struct LINKCFG_DataDrv_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   baseChnlId ;
    Uint32                   numChannels ;
    Uint32                   maxBufSize ;
    Uint32                   memEntry ;
    Uint32                   poolId ;
    Uint32                   queuePerChnl ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
    Uint32                   arg1 ;
    Uint32                   arg2 ;
} LINKCFG_DataDrv ;


/** ============================================================================
 *  @name   LINKCFG_Mqt
 *
 *  @desc   This structure defines the configuration structure for the Message
 *          Queue Transport.
 *
 *  @field  name
 *              Name of the MQT.
 *  @field  memEntry
 *              MEM entry id for the memory area used by this MQT.
 *              This field is optional and can be specified as -1 in case
 *              a MEM entry is not required.
 *  @field  maxMsgSize
 *              Maximum size of message supported by MQT. May be -1 if there is
 *              no limit on maximum message size for the MQT.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MQT.
 *  @field  ipsEventNo
 *              IPS Event number associated with MQT (if any). A value of -1
 *              indicates that no IPS is required by the MQT.
 *  @field  arg1
 *              First optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  @field  arg2
 *              Second optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  ============================================================================
 */
typedef struct LINKCFG_Mqt_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   memEntry ;
    Uint32                   maxMsgSize ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
    Uint32                   arg1 ;
    Uint32                   arg2 ;
} LINKCFG_Mqt ;


/** ============================================================================
 *  @name   LINKCFG_RingIo
 *
 *  @desc   This structure defines the configuration structure for the RingIO
 *          component.
 *
 *  @field  name
 *              Name of the RingIO.
 *  @field  memEntry
 *              MEM entry ID for the memory area used by this RingIO.
 *  @field  maxEntries
 *              Maximum number of RingIO instances supported by the RingIO.
 *  @field  ipsId
 *              ID of the IPS to be used.
 *  @field  ipsEventNo
 *              IPS Event number associated with the RingIO.
 *  ============================================================================
 */
typedef struct LINKCFG_RingIo_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   memEntry ;
    Uint32                   maxEntries ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
} LINKCFG_RingIo ;


/** ============================================================================
 *  @name   LINKCFG_MpList
 *
 *  @desc   This structure defines the configuration structure for the MPLIST
 *          component.
 *
 *  @field  name
 *              Name of the MPLIST.
 *  @field  memEntry
 *              MEM entry ID for the memory area used by this MPLIST.
 *  @field  maxEntries
 *              Maximum number of MPLIST instances supported by the MPLIST.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MPLIST.
 *  @field  ipsEventNo
 *              IPS Event number associated with MPLIST (if any). A value of -1
 *              indicates that no IPS is required by the MPLIST.
 *  ============================================================================
 */
typedef struct LINKCFG_MpList_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   memEntry ;
    Uint32                   maxEntries ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
} LINKCFG_MpList ;


/** ============================================================================
 *  @name   LINKCFG_Mpcs
 *
 *  @desc   This structure defines the configuration structure for the MPCS
 *          component.
 *
 *  @field  name
 *              Name of the MPCS.
 *  @field  memEntry
 *              MEM entry ID for the memory area used by this MPCS.
 *  @field  maxEntries
 *              Maximum number of MPCS instances supported by the MPCS.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MPCS.
 *  @field  ipsEventNo
 *              IPS Event number associated with MPCS (if any). A value of -1
 *              indicates that no IPS is required by the MPCS.
 *  ============================================================================
 */
typedef struct LINKCFG_Mpcs_tag {
    Char8                    name [DSP_MAX_STRLEN] ;
    Uint32                   memEntry ;
    Uint32                   maxEntries ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
} LINKCFG_Mpcs ;


/** ============================================================================
 *  @name   LINKCFG_Log
 *
 *  @desc   This structure defines the configuration structure for the LOG
 *          component.
 *
 *  @field  gdMsgqPut
 *              GPP->DSP MSG Transfer  - MSGQ_put call.
 *  @field  gdMsgqSendInt
 *              GPP->DSP MSG Transfer  - GPP sends interrupt.
 *  @field  gdMsgqIsr
 *              GPP->DSP MSG Transfer  - DSP receives interrupt.
 *  @field  gdMsgqQue
 *              GPP->DSP MSG Transfer  - Message queued at DSP.
 *  @field  dgMsgqPut
 *              DSP->GPP MSG Transfer  - MSGQ_put call.
 *  @field  dgMsgqSendInt
 *              DSP->GPP MSG Transfer  - DSP sends interrupt.
 *  @field  dgMsgqIsr
 *              DSP->GPP MSG Transfer  - GPP receives interrupt.
 *  @field  dgMsgqQue
 *              DSP->GPP MSG Transfer  - Message queued at GPP.
 *  @field  gdChnlIssueStart
 *              GPP->DSP CHNL Transfer - Entering inside ISSUE call.
 *  @field  gdChnlIssueQue
 *              GPP->DSP CHNL Transfer - ISSUE: Buffer is queued in internal
 *              structure on GPP.
 *  @field  gdChnlIssueCompl
 *              GPP->DSP CHNL Transfer - ISSUE call completed.
 *  @field  gdChnlXferStart
 *              GPP->DSP CHNL Transfer - Initiating a buffer transfer by GPP.
 *  @field  gdChnlXferProcess
 *              GPP->DSP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  gdChnlXferCompl
 *              GPP->DSP CHNL Transfer - Buffer transfer is complete.
 *  @field  gdChnlReclStart
 *              GPP->DSP CHNL Transfer - Entering RECLAIM call.
 *  @field  gdChnlReclPend
 *              GPP->DSP CHNL Transfer - RECLAIM: Wait on a semaphore.
 *  @field  gdChnlReclPost
 *              GPP->DSP CHNL Transfer - RECLAIM: Posting the Semaphore.
 *  @field  gdChnlReclCompl
 *              GPP->DSP CHNL Transfer - RECLAIM call completed.
 *  @field  dgChnlIssueQue
 *              DSP->GPP CHNL Transfer - ISSUE: Buffer is queued in internal
 *              structure on DSP.
 *  @field  dgChnlXferStart
 *              DSP->GPP CHNL Transfer - Initiating a buffer transfer by DSP.
 *  @field  dgChnlXferProcess
 *              DSP->GPP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  dgChnlXferCompl
 *              DSP->GPP CHNL Transfer - Buffer transfer is complete.
 *  @field  dgChnlReclPend
 *              DSP->GPP CHNL Transfer - RECLAIM: Wait on a semaphore.
 *  @field  dgChnlReclPost
 *              DSP->GPP CHNL Transfer - RECLAIM: Posting the Semaphore.
 *  @field  msgIdRangeStart
 *              MSG ID range: lower limit.
 *  @field  msgIdRangeEnd
 *              MSG ID range: upper limit.
 *  ============================================================================
 */
typedef struct LINKCFG_Log_tag {
    Bool                     gdMsgqPut ;
    Bool                     gdMsgqSendInt ;
    Bool                     gdMsgqIsr ;
    Bool                     gdMsgqQue ;

    Bool                     dgMsgqPut ;
    Bool                     dgMsgqSendInt ;
    Bool                     dgMsgqIsr ;
    Bool                     dgMsgqQue ;

    Bool                     gdChnlIssueStart ;
    Bool                     gdChnlIssueQue ;
    Bool                     gdChnlIssueCompl ;

    Bool                     gdChnlXferStart ;
    Bool                     gdChnlXferProcess ;
    Bool                     gdChnlXferCompl ;

    Bool                     gdChnlReclStart ;
    Bool                     gdChnlReclPend ;
    Bool                     gdChnlReclPost ;
    Bool                     gdChnlReclCompl ;

    Bool                     dgChnlIssueQue ;

    Bool                     dgChnlXferStart ;
    Bool                     dgChnlXferProcess ;
    Bool                     dgChnlXferCompl ;

    Bool                     dgChnlReclPend ;
    Bool                     dgChnlReclPost ;

    Uint32                   msgIdRangeStart ;
    Uint32                   msgIdRangeEnd ;
} LINKCFG_Log ;


/** ============================================================================
 *  @name   LINKCFG_DspConfig
 *
 *  @desc   Defines the object containing all configuration items for DSPs.
 *
 *  @field  dspObject
 *              DSP object.
 *  @field  numDrvs
 *              Number of link Drv specified in configuration database.
 *  @field  linkDrvObjects
 *              Pointer to the array of link Drv objects.
 *  @field  numMemTables
 *              Number of MEM tables specified in configuration database.
 *  @field  memTables
 *              Pointer to the array of memory information table arrays.
 *  @field  numIpsTables
 *              Number of IPS tables.
 *  @field  ipsTables
 *              Pointer to the array of IPS table arrays.
 *  @field  numPoolTables
 *              Number of POOL tables.
 *  @field  poolTables
 *              Pointer to the array of POOL table arrays.
 *  @field  numDataTables
 *              Number of data tables.
 *  @field  dataTables
 *              Pointer to the array data table arrays.
 *  @field  numMqts
 *              Number of Message Queue Transports.
 *  @field  mqtObjects
 *              Pointer to the array of MQT objects.
 *  @field  numRingIo
 *              Number of RingIO tables.
 *  @field  ringIoObjects
 *              Pointer to the array of RingIO tables.
 *  @field  numMpList
 *              Number of MPLIST tables.
 *  @field  mplistObjects
 *              Pointer to the array of MPLIST tables.
 *  @field  numMpcs
 *              Number of MPCS tables.
 *  @field  mpcsObjects
 *              Pointer to the array of MPCS tables.
 *  @field  logObject
 *              Pointer to the LOG object.
 *  ============================================================================
 */
typedef struct LINKCFG_DspConfig_tag {
    LINKCFG_Dsp *            dspObject  ;
    Uint32                   numDrvs ;
    LINKCFG_LinkDrv *        linkDrvObjects ;
    Uint32                   numMemTables ;
    LINKCFG_MemEntry **      memTables ;

    Uint32                   numIpsTables ;
    LINKCFG_Ips **           ipsTables ;

    Uint32                   numPoolTables ;
    LINKCFG_Pool **          poolTables ;

    Uint32                   numDataTables ;
    LINKCFG_DataDrv **       dataTables ;

    Uint32                   numMqts ;
    LINKCFG_Mqt *            mqtObjects ;

    Uint32                   numRingIo ;
    LINKCFG_RingIo *         ringIoObjects ;

    Uint32                   numMpList ;
    LINKCFG_MpList *         mplistObjects ;

    Uint32                   numMpcs ;
    LINKCFG_Mpcs *           mpcsObjects ;

    LINKCFG_Log *            logObject ;
} LINKCFG_DspConfig ;


/** ============================================================================
 *  @name   LINKCFG_Object
 *
 *  @desc   This structure defines the configuration structure for the system.
 *
 *  @field  gppObject
 *              Pointer to the GPP specific configuration object.
 *  @field  linkObjects
 *              DSP/BIOS LINK configuration structures.
 *  ============================================================================
 */
typedef struct LINKCFG_Object_tag {
    LINKCFG_Gpp *       gppObject   ;
    LINKCFG_DspConfig * dspConfigs [MAX_DSPS] ;
} LINKCFG_Object ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LINKCFGDEFS_H) */
