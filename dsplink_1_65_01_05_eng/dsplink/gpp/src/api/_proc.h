/** ============================================================================
 *  @file   _proc.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines internal structures and/or functions for the PROC component.
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


#if !defined (_PROC_H)
#define _PROC_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>
#if defined (POOL_COMPONENT)
#include <pooldefs.h>
#endif /* if defined (POOL_COMPONENT) */
#include <_sync_usr.h>

/*  ----------------------------------- Profiling                       */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   PROC_CurStatus
 *
 *  @desc   This structure defines the current status of the PROC component for
 *          each process.
 *
 *  @field  isSetup
 *              Indicates whether PROC has been setup in this process.
 *  @field  isAttached
 *              Indicates whether PROC has been attached in this process for the
 *              specified processor ID.
 *  @field  isStarted
 *              Indicates whether PROC has been started in this process for the
 *              specified processor ID.
 *  @field  poolIsOpened
 *              Indicates whether POOL has been opened in this process for the
 *              specified pool ID. Only defined if POOL component is enabled.
 *  @field  mqtIsOpened
 *              Indicates whether MSGQ transport has been opened in this process
 *              for the specified processor ID. Only defined if MSGQ component
 *              is enabled.
 *  ============================================================================
 */
typedef struct PROC_CurStatus_tag {
    Uint32  isSetup ;
    Uint32  isAttached   [MAX_DSPS] ;
    Uint32  isStarted    [MAX_DSPS] ;
#if defined (POOL_COMPONENT)
    Uint32  poolIsOpened [MAX_DSPS][MAX_POOLENTRIES] ;
#endif /* if defined (POOL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
    Uint32  mqtIsOpened  [MAX_DSPS] ;
#endif /* if defined (MSGQ_COMPONENT) */
} PROC_CurStatus ;

/** ============================================================================
 *  @name   PROC_Object
 *
 *  @desc   This structure defines the PROC object, which contains state
 *          information required by the PROC user-side component.
 *
 *  @field  syncCsObj
 *              Mutex for protecting PROC operations in user-space.
 *  @field  curStatus
 *              Current status for the components for each process.
 *  ============================================================================
 */
typedef struct PROC_Object_tag {
    SYNC_USR_CsObject * syncCsObj ;
    PROC_CurStatus      curStatus ;
} PROC_Object ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PROC_H) */
