/** ============================================================================
 *  @file   linklog.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines constants, data types and structures used by
 *          DSP/BIOS(TM) LINK LOG component.
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


#if !defined (LINKLOG_H)
#define LINKLOG_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DsplinkLogEventType_tag
 *
 *  @desc   Defines the types of event available for LOG.
 *
 *  @field  GD_MSGQ_PUT
 *              GPP->DSP MSG Transfer  - MSGQ_put call.
 *  @field  GD_MSGQ_SND
 *              GPP->DSP MSG Transfer  - GPP sends interrupt.
 *  @field  GD_MSGQ_ISR
 *              GPP->DSP MSG Transfer  - DSP receives interrupt.
 *  @field  GD_MSGQ_QUE
 *              GPP->DSP MSG Transfer  - Message queued at DSP.
 *  @field  DG_MSGQ_PUT
 *              DSP->GPP MSG Transfer  - MSGQ_put call.
 *  @field  DG_MSGQ_SND
 *              DSP->GPP MSG Transfer  - DSP sends interrupt.
 *  @field  DG_MSGQ_ISR
 *              DSP->GPP MSG Transfer  - GPP receives interrupt.
 *  @field  DG_MSGQ_QUE
 *              DSP->GPP MSG Transfer  - Message queued at GPP.
 *  @field  GD_CHNL_I_START
 *              GPP->DSP CHNL Transfer - Entring inside ISSUE call.
 *  @field  GD_CHNL_I_QUE
 *              GPP->DSP CHNL Transfer - Buffer is queued in internal structure
 *              on GPP.
 *  @field  GD_CHNL_I_COMPLETE
 *              GPP->DSP CHNL Transfer - ISSUE call completed.
 *  @field  GD_CHNL_XFER_START
 *              GPP->DSP CHNL Transfer - Initiating a buffer transfer by GPP.
 *  @field  GD_CHNL_XFER_PROCESSING
 *              GPP->DSP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  GD_CHNL_XFER_COMPLETE
 *              GPP->DSP CHNL Transfer - Buffer transfer is complete.
 *  @field  GD_CHNL_R_START
 *              GPP->DSP CHNL Transfer - Entring RECLAIM call.
 *  @field  GD_CHNL_R_PEND
 *              GPP->DSP CHNL Transfer - Wait on a semaphore.
 *  @field  GD_CHNL_R_POST
 *              GPP->DSP CHNL Transfer - posting the Semaphore.
 *  @field  GD_CHNL_R_COMPLETE
 *              GPP->DSP CHNL Transfer - RECLAIM call completed.
 *  @field  DG_CHNL_I_QUE
 *              DSP->GPP CHNL Transfer - Buffer is queued in internal structure
 *              on DSP.
 *  @field  DG_CHNL_XFER_START
 *              DSP->GPP CHNL Transfer - Initiating a buffer transfer by DSP.
 *  @field  DG_CHNL_XFER_PROCESSING
 *              DSP->GPP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  DG_CHNL_XFER_COMPLETE
 *              DSP->GPP CHNL Transfer - Buffer transfer is complete.
 *  @field  DG_CHNL_R_PEND
 *              DSP->GPP CHNL Transfer - Wait on a semaphore.
 *  @field  DG_CHNL_R_POST
 *              DSP->GPP CHNL Transfer - posting the Semaphore.
 *  ============================================================================
 */
typedef enum DsplinkLogEventType_tag {
    GD_MSGQ_PUT             = 0u,
    GD_MSGQ_SND             = 1u,
    GD_MSGQ_ISR             = 2u,
    GD_MSGQ_QUE             = 3u,
    DG_MSGQ_PUT             = 4u,
    DG_MSGQ_SND             = 5u,
    DG_MSGQ_ISR             = 6u,
    DG_MSGQ_QUE             = 7u,
    GD_CHNL_I_START         = 8u,
    GD_CHNL_I_QUE           = 9u,
    GD_CHNL_I_COMPLETE      = 10u,
    GD_CHNL_XFER_START      = 11u,
    GD_CHNL_XFER_PROCESSING = 12u,
    GD_CHNL_XFER_COMPLETE   = 13u,
    GD_CHNL_R_START         = 14u,
    GD_CHNL_R_PEND          = 15u,
    GD_CHNL_R_POST          = 16u,
    GD_CHNL_R_COMPLETE      = 17u,
    DG_CHNL_I_QUE           = 18u,
    DG_CHNL_XFER_START      = 19u,
    DG_CHNL_XFER_PROCESSING = 20u,
    DG_CHNL_XFER_COMPLETE   = 21u,
    DG_CHNL_R_PEND          = 22u,
    DG_CHNL_R_POST          = 23u
} DsplinkLogEventType ;


/** ============================================================================
 *  @func   DSPLINKLOG_LogEvent
 *
 *  @desc   Logs a event
 *
 *  @arg    eventType
 *              Event type.
 *  @arg    id
 *              msg Id used to differentiate SoCrates' msgs.
 *  @arg    queId
 *              queue Id.
 *  @arg    addr
 *              Address of buffer.
 *  @arg    size
 *              size of the buffer.
 *  @arg    arg1
 *              Argument1.
 *  @arg    arg2
 *              Argument2.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Void
DSPLINKLOG_LogEvent (IN DsplinkLogEventType eventType,
                     IN Uint32 id,
                     IN Uint32 queId,
                     IN Uint32 addr,
                     IN Uint32 size,
                     IN Uint32 arg1,
                     IN Uint32 arg2) ;


#if defined (__cplusplus)
}
#endif /* extern "C" */


#endif /* LINKLOG_H */
