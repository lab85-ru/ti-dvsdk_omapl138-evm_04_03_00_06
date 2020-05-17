/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Use of this software is controlled by the terms and conditions found in
 * the New BSD Software License Agreement, which is reproduced in its
 * entirity below.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/*
 *  ======== LogTrack.h =======
 */

#include <std.h>
#include <log.h>
#include <tsk.h>

#ifndef LogTrack_
#define LogTrack_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The 'track' LOG is used to track CPU load, EMIF counts,
 *  etc.
 *
 *  Format for this LOG:
 *      <sequence #>
 *      CPU load
 *      User Value 1
 *      User Value 2
 *      User Value 3
 *      User Value 4
 *      Time Stamp (8 bytes) 
 */
extern LOG_Obj ti_bios_log_track;

/*
 *  We use a separate LOG to handle TSK create/exit/delete events to make sure
 *  that we don't miss any of these.
 *
 *  Format for this LOG:
 *      <sequence #>
 *      task handle
 *      name (4 bytes)
 *      name cont'd (4 bytes)
 *      name cont'd (4 bytes)
 *      name cont'd (4 bytes)   NOTE!  not NULL terminated if strlen > 15!
 *      Time Stamp  (8 bytes) 
 */
extern LOG_Obj ti_bios_log_create;

/*
 *  ======== LogTrack_createHook ========
 *  connect to TSK create hook
 */
extern Void LogTrack_createHook(TSK_Handle task);

/*
 *  ======== LogTrack_init ========
 *  Initialization function.
 */
extern Void LogTrack_init(Void);

/*
 *  ======== LogTrack_prdFxn ========
 *  add CPU load, EMIF, etc. info to the track log
 */
extern Void LogTrack_prdFxn(Void);

/*
 *  ======== LogTrack_setTrackAddrs ========
 *  set the values of the LogTrack_config struct.  Send address 0x0 for a don't
 *  care.
 */
extern Void LogTrack_setTrackAddrs(Uint32 *addr1, Uint32 *addr2, Uint32 *addr3,
        Uint32 *addr4);

#ifdef __cplusplus
}
#endif

#endif

/* 
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:23; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

