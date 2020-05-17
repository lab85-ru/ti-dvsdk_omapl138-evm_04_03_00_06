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
 *  ======== LogAux.c ========
 */

#include <ti/bios/include/std.h>

#include <ti/bios/include/log.h>

#include "LogAux.h"

/* Assumes 64x 8-word timestamp LOG! */
#define LOGELEMSIZE             8

#define BYTESPERWORD            sizeof(Uint32)

#ifdef LOGSERVER_DEBUG
extern LOG_Obj ti_bios_log_debugTrace;
#endif

extern LOG_Obj ti_bios_log_create;

extern far Void * LOG_A_TABBEG;
extern far Void * LOG_A_TABEND;

/* global data structure used for remembering log info between transfers */
typedef struct ScanLogInfo {
    Bool createLogScanComplete; /* TRUE if the create log has been scanned */
    Uint32 scanMask;    /* used to save the list of logs requested to be read */
    Uint32 currLogId;    /* used to save id of log we left off on */
    Uint32 firstEnabledLogId; /* id of the first log enabled by logMask */
    Uint32 currentOffset; /* save the # of words of log data read so far */

    /* Log table information */
    LOG_Obj *logTab;
    LOG_Obj *logTabEnd;
    Uint32  numLogs;    /* number of logs in the system */

    /* special logs */
    Uint32 createLogId;
} ScanLogInfo;

static ScanLogInfo scanloginfo;

typedef struct Header {
    Int32       id;
    Uint32      size;
} Header;

/*
 *  ======== LogAux_init ========
 *  Initialization.
 */
Void LogAux_init(Void)
{
#ifdef LOGSERVER_DEBUG
    Int i;
#endif

    scanloginfo.currLogId = 0;
    scanloginfo.firstEnabledLogId = 0;
    scanloginfo.currentOffset = 0;
    scanloginfo.scanMask = 0;
    scanloginfo.createLogScanComplete = 0;
    scanloginfo.createLogId = 0xffffffff;

    /* define the log table */
    scanloginfo.logTab = (LOG_Obj *)&LOG_A_TABBEG;
    scanloginfo.logTabEnd = (LOG_Obj *)&LOG_A_TABEND;
    scanloginfo.numLogs = ((Uint32)scanloginfo.logTabEnd -
            (Uint32)scanloginfo.logTab) / sizeof(LOG_Obj);

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: entered LogAux_init()");
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: logTab = %x\n",
            scanloginfo.logTab);
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: logTabEnd = %x\n",
            scanloginfo.logTabEnd);
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: numLogs = %d\n",
            scanloginfo.numLogs);
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: create log addr = %x\n",
            &ti_bios_log_create);

    for (i = 0; i < scanloginfo.numLogs; i++) {
        LOG_printf(&ti_bios_log_debugTrace,
                "LogAux: logTab[%d] = %x", i, scanloginfo.logTab[i]);
        LOG_printf(&ti_bios_log_debugTrace, "\tLogAux: lenmask = %d",
                scanloginfo.logTab[i].lenmask);
    }
#endif

    /* Find the indices for the special create log */
    scanloginfo.createLogId = ((Uint32)&ti_bios_log_create -
            (Uint32)scanloginfo.logTab) / sizeof(LOG_Obj);
}

/*
 *  ======== LogAux_disable ========
 */
Void LogAux_disable(Uint32 logMask)
{
    Uint32 mask = 1;
    Int i;

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: entered LogAux_disable()\n");
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: received mask = %x\n", logMask);
#endif

    /* disable each log specified in the mask */
    for (i = 0; i < scanloginfo.numLogs; i++) {
        if (logMask & mask) {
            LOG_disable(&scanloginfo.logTab[i]);
        }
        mask <<= 1;
    }

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace,
        "LogAux: exiting LogAux_disable function.\n");
#endif
}

/*
 *  ======== LogAux_enable ========
 */
Void LogAux_enable(Uint32 logMask)
{
    Uint32 mask = 1;
    Int i;

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: entered LogAux_enable\n");
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: received mask = %x\n", logMask);
#endif

    for (i = 0; i < scanloginfo.numLogs; i++) {
        if (logMask & mask) {
            LOG_enable(&scanloginfo.logTab[i]);
        }
        mask <<= 1;
    }

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace,
        "LogAux: exiting LogAux_enable function.\n");
#endif
}

/*
 *  ======== LogAux_flush ========
 */
Void LogAux_flush(Uint32 logMask)
{
    Uint32 mask = 1;
    Int i;
    Uns previousLogState;
    Uint32 *src;
    Uint32 *srcEnd;

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: entered LogAux_flush\n");
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: received mask = %x\n", logMask);
#endif

    for (i = 0; i < scanloginfo.numLogs; i++) {
        if (logMask & mask) {
            /*
             *  Save state of this log (enabled or disabled), in order to
             *  restore it later.
             */
            previousLogState = scanloginfo.logTab[i].flag;

            /*
             *  Disable the log so no data is written to it while flushing.
             *  Don't redundantly disable the log if it's already disabled!
             */
            if (previousLogState == 0) {  /* if log enabled */
                LOG_disable(&scanloginfo.logTab[i]);
            }

            /* Get pointer to the beginning of the log */            
            src = (Uint32 *)scanloginfo.logTab[i].bufbeg;

            /* ... and the end of the log */
            srcEnd = (Uint32 *)scanloginfo.logTab[i].bufbeg +
                    (scanloginfo.logTab[i].lenmask + 1) / BYTESPERWORD;

            /* flush the log - overwrite each word with 0xFFFFFFFF */
            while ((src < srcEnd)) {

                /* skip words that are already empty */
                if (*src == 0xffffffff) {
                    src++;
                    continue;
                }
                else { /* empty this word of the log */
                    *src++ = 0xffffffff;
                }
            }

            /*
             *  Restore the state of this log to enabled, if enabled previously.
             */
            if (previousLogState == 0) { /* log enabled previously */
                LOG_enable(&scanloginfo.logTab[i]);
            }
        }
        mask <<= 1;
    }

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace,
        "LogAux: exiting LogAux_flush function.\n");
#endif
}

/*
 *  ======== LogAux_open ========
 */
Void LogAux_open(Uint32 logMask)
{
    Int i;
    Uint32 mask;

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogAux: LogAux_read(), logMask = %d\n",
            logMask);
#endif

    mask = 1;

    /* save the log scan mask sent by the user */   
    scanloginfo.scanMask = logMask;
        scanloginfo.currLogId = scanloginfo.numLogs;
        scanloginfo.firstEnabledLogId = scanloginfo.numLogs;
    scanloginfo.createLogScanComplete = FALSE;

    /* find the id of the first enabled log in scan mask */
        for (i = 0; i < scanloginfo.numLogs; i++) {
        if (scanloginfo.scanMask & mask) {
            scanloginfo.firstEnabledLogId = i;
            scanloginfo.currentOffset = 0;
            break;
        }
        mask <<= 1;
    }
    mask = 1 << scanloginfo.createLogId;
    if (scanloginfo.scanMask & mask) {
        scanloginfo.currLogId = scanloginfo.createLogId;
    }
    else {
        scanloginfo.currLogId = scanloginfo.firstEnabledLogId;
    }
}
/*
 *  ======== LogAux_read ========
 *  Copy log data into a buffer.
 *  Note: size must be an Int* because it is sent to the NDK API send(), which
 *  must take an integer type.
 */
Int LogAux_read(Char *dataBuffer, Int *size)
{
    Uint32 mask;
    Uint32 msgPayloadSize;
    Uint32 headerPayloadSize;
    Uint32 currLogId;
    Uint32 *src;
    Uint32 *srcEnd;
    Uint32 *dst;
    Uint32 *dstEnd;
    Int status;
    Int i;
    Header *header;

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "##########################################\n");
#endif

    status = 0;
    msgPayloadSize = 0;
    mask = 1;

    /* set the destination buffer, used to copy log data into: */
    dst = (Uint32 *)dataBuffer;

    /*
     *      Set the end marker of the destination buffer.
     *
     *  Log data will be copied into this buffer until it fills up (or until
     *  there's no more data).  (Subtract the size of a log header to save room
     *  at the end of the buffer for the "-1 0" end code).
     */
    dstEnd = dst + *size / BYTESPERWORD - sizeof(Header) / BYTESPERWORD;

    /* Loop over each log id specifed by the scan mask. */
    currLogId = scanloginfo.currLogId;
    while (currLogId < scanloginfo.numLogs &&
            (dst + sizeof(Header) / BYTESPERWORD + LOGELEMSIZE) < dstEnd) {

        mask = 1 << currLogId;

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace, "LogAux: mask = %x\n", mask);
#endif

        /* skip logs not enabled by scanmask */
        if (!(scanloginfo.scanMask & mask)) {
            currLogId++;
            continue;
        }

        /*
         *  Only scan the create log once.
         *
         *  If the create log is enabled, then it will be the first log scanned.
         *  The following check is to ensure that it is not re-scanned during
         *  this log cycle.
         */
        if (currLogId == scanloginfo.createLogId &&
                scanloginfo.createLogScanComplete == TRUE) {
            /* if we've already scanned the create log this cycle, skip it */
            currLogId++;
            continue;
        }
        
        /* set the current log ID */
        scanloginfo.currLogId = currLogId;

        /*
         *  Get a pointer to the log buffer we want to extract data from.
         *  Offset is used to pick up where we left off reading from previously
         *  in that log.
         */ 
        src = (Uint32 *)scanloginfo.logTab[currLogId].bufbeg +
                scanloginfo.currentOffset;

        /* set the end position of the log we're reading from. */
        srcEnd = (Uint32 *)scanloginfo.logTab[currLogId].bufbeg +
                (scanloginfo.logTab[currLogId].lenmask + 1) / BYTESPERWORD;

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace, "LogAux: src = %x, srcEnd = %x\n", src, srcEnd);
#endif
        /*
         *    Pre-pend a header into the beginning of the destination buffer.
         *
         *  The header will contain information about the payload of data which
         *  follows it.  The log ID and payload size will be stored in the
         *  header.
         */
        header = (Header *)dst;
        dst = dst + sizeof(Header) / BYTESPERWORD;

        /*
         *  Copy the log data from the log buffers into the destination buffer.
         *
         *  This while loop will continue until the destination buffer has been
         *  filled, or until the end of the current log buffer has been reached.
         */
        headerPayloadSize = 0;
        while ((src < srcEnd) && ((dst + LOGELEMSIZE) < dstEnd)) {
            /*
             * Skip empty records.
             *
             * Do not disable interrupts.  If an entry shows up in the
             * meantime, we'll get it next time.
             */
            if (*src == 0xffffffff) {
                src += LOGELEMSIZE;
                scanloginfo.currentOffset += LOGELEMSIZE;
                continue;
            }

            _disable_interrupts();

            /* The "create" log is a special case */
            if (currLogId != scanloginfo.createLogId) {
                for (i = 0; i < LOGELEMSIZE; i++) {
                    *dst++ = *src;
                    *src++ = 0xffffffff;          /* mark buffer empty */
                }
            }
            else {
                for (i = 0; i < LOGELEMSIZE; i++) {
                    *dst++ = *src++;
                }
            }

            _enable_interrupts();

            scanloginfo.currentOffset += LOGELEMSIZE;        /* # words read */
            headerPayloadSize += LOGELEMSIZE * BYTESPERWORD; /* # bytes read */
        }

        if (headerPayloadSize > 0) {
            /* Record the log ID and payload size for this buffer of data. */
            header->id = scanloginfo.currLogId;
            header->size = headerPayloadSize;
            msgPayloadSize += sizeof(Header) + headerPayloadSize;
        }
        else {
             /* no data was copied to dst, remove the header added previously */
             dst -= sizeof(Header) / BYTESPERWORD;
        }

        if (src == srcEnd) {
            /*
             *     Get ready for the next LOG.
             *
             *  We have reached the end of the current log src buffer.
             *  (The entire content of the current log has been read, so we are
             *  done with the current log).  In this case, we need to reset the
             *  saved log offset, logsize, and update the mask.
             */

            /* once the create log has been scanned, scan all other logs */
            if (currLogId == scanloginfo.createLogId) {
                /* set flag indicating create log has been scanned this cycle */
                scanloginfo.createLogScanComplete = TRUE;

                /* set log id to the first enabled log */
                currLogId = scanloginfo.firstEnabledLogId;
            }
            else {
                /* go on to the next log */
                currLogId++; 
            }
            /* save log id incase loop ends after this due to full dst buf */
            scanloginfo.currLogId = currLogId;

            /* reset log values */
            scanloginfo.currentOffset = 0;
            headerPayloadSize = 0;
        }
    }

    /* set the return value to the appropriate cmd */
    if (currLogId < scanloginfo.numLogs) {
        status = 1;
    }
    else {
        /* we've completed this log cycle */
        status = 0;

        /* Add "-1 0" to end of data buffer signaling end of log cycle */
        header = (Header *)dst;
        header->id = -1;
        header->size = 0;

        /* increment the size count for the special [-1,0] end header */
        msgPayloadSize += sizeof(Header);
    }

    /* update the total payload size */
    *size = msgPayloadSize;

    return status;
}

/*
 *  ======== LogAux_close ========
 */
Void LogAux_close(Void)
{
}

/*
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:22; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

