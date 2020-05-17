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
 *  ======== LogTrack.c =======
 */

#include <ti/bios/include/std.h>
#include <ti/bios/include/log.h>
#include <ti/bios/include/tsk.h>

#include <string.h>

#include <ti/bios/utils/Load.h>
#include "LogTrack.h"

/* Create default global instance of LogTrack_config. */
static Uint32 * trackAddrs[4] = {0x0, 0x0, 0x0, 0x0};

/*
 *  ======== LogTrack_createHook ========
 */
Void LogTrack_createHook(TSK_Handle task)
{
    String taskName;

    /* need to copy to Uint32 to make sure aligned */
    Uint32 name[4] = {0, 0, 0, 0};

/* quit build if trying to build for big endian */
#ifdef xdc_target__bigEndian
#error "The following code will not work for big endian"
#endif
    /* Note: the following string copy operation is not supported for BE! */
    taskName = TSK_getname(task);
    if (taskName != NULL) {
        strncpy((char *)name, taskName, sizeof(name));
    }

    LOG_event5(&ti_bios_log_create,
            (Arg)task, (Arg)name[0], (Arg)name[1], (Arg)name[2], (Arg)name[3]);
}

/*
 *  ======== LogTrack_init ========
 */
Void LogTrack_init()
{
    /* call Load_init() - this is required in order to use Load_getcpuload() */
    Load_init();
}

/*
 *  ======== LogTrack_prdFxn ========
 */
Void LogTrack_prdFxn()
{
    Uint32 trackConfigVal1;
    Uint32 trackConfigVal2;
    Uint32 trackConfigVal3;
    Uint32 trackConfigVal4;

    /*
     *  Check for addresses set to "0x0" in the trackAddrs array.  If an
     *  address is "0x0", then it's a don't care, and print "0" for this arg.
     */
    trackConfigVal1 = (trackAddrs[0] == 0x0) ? 0 : 
        *trackAddrs[0];

    trackConfigVal2 = (trackAddrs[1] == 0x0) ? 0 : 
        *trackAddrs[1];

    trackConfigVal3 = (trackAddrs[2] == 0x0) ? 0 : 
        *trackAddrs[2];

    trackConfigVal4 = (trackAddrs[3] == 0x0) ? 0 : 
        *trackAddrs[3];

    LOG_event5(&ti_bios_log_track, Load_getcpuload(), (Arg)trackConfigVal1,
            (Arg)trackConfigVal2, (Arg)trackConfigVal3,
            (Arg)trackConfigVal4);
}

/*
 *  ======== LogTrack_setTrackAddrs ========
 *  set the values of the LogTrack_config structure.
 */
Void LogTrack_setTrackAddrs(Uint32 *addr1, Uint32 *addr2, Uint32 *addr3,
        Uint32 *addr4)
{
    trackAddrs[0] = addr1;
    trackAddrs[1] = addr2;
    trackAddrs[2] = addr3;
    trackAddrs[3] = addr4;
}

/*
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:22; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

