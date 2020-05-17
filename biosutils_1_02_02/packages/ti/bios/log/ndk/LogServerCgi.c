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
 *  ======== LogServerCgi.c ========
 */


#include <ti/bios/include/std.h>

#include <ti/bios/include/gbl.h>
#include <ti/bios/include/log.h>
#include <ti/bios/include/tsk.h>

#include <stdlib.h>
#include <stkmain.h>
#include <_nettool.h>
#include <_netctrl.h>

#include <ti/bios/log/support/LogAux.h>
#include <ti/bios/log/support/LogTrack.h>
#include "LogServerCgi.h"

/* global flag used to end streaming of log data */
static Bool LogServerCgi_continueStreaming;

#ifdef LOGSERVER_DEBUG
extern LOG_Obj ti_bios_log_debugTrace;
#endif

/* declare external reference to struct containing user configured priority */
extern LogServerCgi_Config LogServerCgi_config;

/* data structure used for storing log scan mask and polling frequency */
typedef struct StreamCaptData {
    Uint32 scanMask;    /* describes the logs requested to be read */
    Uint32 pollFrequency; /* rate to sleep between scans (stream mode only) */
} StreamCaptData;

#define html(str) httpSendClientStr(htmlSock, (char *)str)

#define DISABLELOGS 0
#define ENABLELOGS  1
#define FLUSHLOGS   2
#define GETCPUSPEED 3
#define READWORD    4
#define WRITEWORD   5

static inline setCgiPriority(Void);

/* redefine function used to parse cgi params here */
static char *cgiParseVars(char PostIn[], int *pParseIndex);

/* CGI functions */
static int cgiCaptureLogs(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiEnableLogging(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiDisableLogging(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiFlushLogs(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiGetCpuSpeed(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiReadWord(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiSetLogTrackAddrs(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiStop(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiStreamLogs(SOCKET htmlSock, int ContentLength, char *pArgs);
static int cgiWriteWord(SOCKET htmlSock, int ContentLength, char *pArgs);

/* functions containing factored CGI code */
static int flushEnableDisableGetCpuCmd(SOCKET htmlSock, int ContentLength,
        char *pArgs, Int cmd);
static int readWriteWordCmd(SOCKET htmlSock, int ContentLength, char *pArgs,
        Int cmd);
static Int streamCaptureCmd(SOCKET htmlSock, Uint32 scanmask,
        Char *dataBuffer);

/*
 *  ======== LogServerCgi_add ========
 */
void LogServerCgi_add(void)
{
    void *pFxn;

    pFxn = (void*) &cgiCaptureLogs;
    efs_createfile("capturelogs.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiEnableLogging;
    efs_createfile("enablelogging.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiDisableLogging;
    efs_createfile("disablelogging.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiGetCpuSpeed;
    efs_createfile("getcpuspeed.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiFlushLogs;
    efs_createfile("logflush.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiReadWord;
    efs_createfile("readword.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiSetLogTrackAddrs;
    efs_createfile("settrackaddrs.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiStop;
    efs_createfile("stopstream.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiStreamLogs;
    efs_createfile("streamlogs.cgi", 0, (UINT8*)pFxn);
    pFxn = (void*) &cgiWriteWord;
    efs_createfile("writeword.cgi", 0, (UINT8*)pFxn);
}

/*
 *  ======== LogServerCgi_remove ========
 */
void LogServerCgi_remove(void)
{
    efs_destroyfile("capturelogs.cgi");
    efs_destroyfile("enablelogging.cgi");
    efs_destroyfile("disablelogging.cgi");
    efs_destroyfile("getcpuspeed.cgi");
    efs_destroyfile("logflush.cgi");
    efs_destroyfile("readword.cgi");
    efs_destroyfile("settrackaddrs.cgi");
    efs_destroyfile("stopstream.cgi");
    efs_destroyfile("streamlogs.cgi");
    efs_destroyfile("writeword.cgi");
}

/*
 * ======== LogServerCgi_init ========
 * Initialization.
 */
Void LogServerCgi_init(Void)
{
    /* initialize log support modules */
    LogAux_init();
    LogTrack_init();
}

/*
 *  ======== cgiCaptureLogs ========
 *  Read all log data for each log specified by logMask and send to host exactly
 *  once.
 */
static int cgiCaptureLogs(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    Int parseIndex;
    int returnCode;
    Char *key;
    Char *dataBuffer;

    /* must use unsigned long type because I'm using strtoul() API */
    unsigned long scanmask;

    int status;

#ifdef LOGSERVER_DEBUG
    LOG_printf( &ti_bios_log_debugTrace, "LogServerCgi: cgiCaptureLogs()");
#endif

    /* change this CGI task's priority to the user configured level */
    setCgiPriority();

#ifdef LOGSERVER_DEBUG
    printf("Capture Logs: pArgs = %s\n", pArgs);
#endif

    /*
     * Process the CGI parameters.
     */
    parseIndex = 0;
    scanmask = 0;
    returnCode = 1;
    key = cgiParseVars( pArgs, &parseIndex );
    while ( parseIndex != -1 ) {
        
#ifdef LOGSERVER_DEBUG
    printf("Capture Logs: key = %s\n", key);
#endif

        if( !strcmp("logMask", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
            printf("Capture Logs: logging key = %s\n", key);
#endif
            scanmask = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("Capture Logs: scanmask = %x\n", scanmask);
#endif

        }
        else {

#ifdef LOGSERVER_DEBUG
            LOG_printf(&ti_bios_log_debugTrace,
                "ERROR: Capture Logs: received unknown command from host\n");
#endif

            /* send error response */
            httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
            return 0;
        }
        key = cgiParseVars( pArgs, &parseIndex );
    }

    /* dynamically create storage for the log data buffer */
    dataBuffer = (Char *)MEM_alloc(LogServerCgi_config.segId,
            LogServerCgi_config.bufSize, 0);

    if (dataBuffer == MEM_ILLEGAL) {
        /*
         * No memory avaiable: send failure to Host
         */
        httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
        html(CRLF);
#ifdef LOGSERVER_DEBUG
            LOG_printf(&ti_bios_log_debugTrace,
                "ERROR: Capture Logs: Memory Allocation failure!\n");
#endif
        return 0;

    }
    else {
        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_APPLET);
        html(CRLF);
    }

    /*
     *      Receive data from log server.
     *
     *  For capture mode, we will only scan and send the data from all the logs
     *  exactly once, so only call streamCaptureCmd() one time.
     */

    status = streamCaptureCmd(htmlSock, (Uint32)scanmask, dataBuffer);

    if (status < 0) {
        returnCode = 0;
#ifdef LOGSERVER_DEBUG
        printf("streamCaptureCmd returned error\n");
#endif
    }

    MEM_free(LogServerCgi_config.segId, dataBuffer,
            LogServerCgi_config.bufSize);

    return returnCode;
}

/*
 * ======== cgiDisableLogging ========
 * Turn off BIOS logging.
 */
static int cgiDisableLogging(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return flushEnableDisableGetCpuCmd(htmlSock, ContentLength, pArgs,
            DISABLELOGS);
}

/*
 * ======== cgiEnableLogging ========
 * Turn on BIOS logging.
 */
static int cgiEnableLogging(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return flushEnableDisableGetCpuCmd(htmlSock, ContentLength, pArgs,
            ENABLELOGS);
}

/*
 * ======== cgiFlushLogs ========
 * Turn on BIOS logging.
 */
static int cgiFlushLogs(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return flushEnableDisableGetCpuCmd(htmlSock, ContentLength, pArgs,
            FLUSHLOGS);
}

/*
 *  ======== cgiGetCpuSpeed ========
 *  Get the speed at which the DSP is running in this program and send to Host.
 */
static int cgiGetCpuSpeed(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return flushEnableDisableGetCpuCmd(htmlSock, ContentLength, pArgs,
            GETCPUSPEED);
}

/*
 * ======== cgiReadWord ========
 * Read a word of data from DSP memory.
 */
static int cgiReadWord(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return (readWriteWordCmd(htmlSock, ContentLength, pArgs, READWORD));
}

/*
 *  ======== cgiParseVars ========
 *  Redefine this function in the library so that the user doesn't have to
 *  include "cgiparse.c" into their program.
 *
 *  Reads input from a CGI post operation in pointed to by PostIn and returns in
 *  sequence a pointer the name and then the value of each post entry. This
 *  function modifies the data in PostIn. It also returns the current parsing
 *  position in the variable pParseIndex.  The parse index must be set to 0 on
 *  initial call.
 */
static char *cgiParseVars(char PostIn[], int *pParseIndex )
{
    int  out;
    int  in;
    char hexch;
    char hexval[3];
    int start;
    char ch;

    /* Get the current parse index. On the first call, it must be zero. */
    in = *pParseIndex;

    hexval[2] = '\0';
    if (in == 0) {
        out = 0;
    }
    else if (in == -1) {
        return NULL;
    }
    else {
        out = ++in;
    }

    start = in;
    while (((ch = PostIn[in]) != '=') && (ch != '&') && (ch != '\0')) {
        if (ch == '+') {
            PostIn[out++] = ' ';
        }
        else if (ch == '%') {
            hexval[0] = PostIn[++in];
            hexval[1] = PostIn[++in];
            hexch = (char) strtol(hexval, NULL, 16);
            PostIn[out++] = hexch;
        }
        else {
            PostIn[out++] = ch;
        }
        in++;
    }

    /* If we got to the end of the string, set the parse index to -1 */
    if (ch == '\0') {
        in = -1;
    }

    /* Null terminate the result string */
    PostIn[out++] = '\0';

    /* Save the value of the current parse index */
    *pParseIndex = in;

    /* Return a pointer to the start of the result string */
    return (&PostIn[start]);
}

/*
 *  ======== cgiSetLogTrackAddrs ========
 *  Set the addresses of the LogTrack_config structure.  The data stored in
 *  these addresses is printed into the ti_bios_log_track log.
 */
static int cgiSetLogTrackAddrs(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    Char *key;
    Int parseIndex;

    /* must use unsigned long type because I'm using strtoul() API */
    unsigned long trackAddrArg1;
    unsigned long trackAddrArg2;
    unsigned long trackAddrArg3;
    unsigned long trackAddrArg4;

    /* these pointers are used for indirection in order to avoid warnings */ 
    Uint32 *trackAddr1;
    Uint32 *trackAddr2;
    Uint32 *trackAddr3;
    Uint32 *trackAddr4;

#ifdef LOGSERVER_DEBUG
    printf( "LogServerCgi: cgiSetLogTrackAddrs: pArgs = %s\n", pArgs);
#endif

    /* change this CGI task's priority to the user configured level */
    setCgiPriority();

    /* initialize variables */
    parseIndex = 0;
    trackAddrArg1 = 0;
    trackAddrArg2 = 0;
    trackAddrArg3 = 0;
    trackAddrArg4 = 0;
    
    /*
     * Process the CGI parameters.
     */
    while ( parseIndex != -1 ) {

        key = cgiParseVars( pArgs, &parseIndex );
#ifdef LOGSERVER_DEBUG
        printf( "LogServerCgi: cgiSetLogTrackAddrs: key = %s\n", key);
#endif

        if( !strcmp("addr1", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
        printf( "LogServerCgi: cgiSetLogTrackAddrs: key = %s\n", key);
#endif

            trackAddrArg1 = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: cgiSetLogTrackAddrs: addr1 = %x\n",
                    trackAddrArg1);
#endif

        }
        else if( !strcmp("addr2", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
        printf( "LogServerCgi: cgiSetLogTrackAddrs: key = %s\n", key);
#endif

            trackAddrArg2 = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: cgiSetLogTrackAddrs: addr2 = %x\n",
                    trackAddrArg2);
#endif

        }
        else if( !strcmp("addr3", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
        printf( "LogServerCgi: cgiSetLogTrackAddrs: key = %s\n", key);
#endif

            trackAddrArg3 = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: cgiSetLogTrackAddrs: addr3 = %x\n",
                    trackAddrArg3);
#endif

        }
        else if( !strcmp("addr4", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
        printf( "LogServerCgi: cgiSetLogTrackAddrs: key = %s\n", key);
#endif

            trackAddrArg4 = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: cgiSetLogTrackAddrs: addr4 = %x\n",
                    trackAddrArg4);
#endif

        }
        else {

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
            "ERROR: readWriteWordCmd: received unknown command from host.\n");
#endif
            /* send error response */
            httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
            html(CRLF);

            return 0;
        }
    }

    /*
     *  Cast each trackAddrArg address sent from Host to pointer.  The extra
     *  cast from 'unsigned long' to 'Uint32' is necessary to avoid compiler
     *  warnings.  Note, these data types must be 'unsigned long' due to the use
     *  of the 'strtoul' function (which in turn is used because it supports
     *  base 16.
     */
    trackAddr1 = (Uint32 *)(Uint32)trackAddrArg1;
    trackAddr2 = (Uint32 *)(Uint32)trackAddrArg2;
    trackAddr3 = (Uint32 *)(Uint32)trackAddrArg3;
    trackAddr4 = (Uint32 *)(Uint32)trackAddrArg4;

    /* call fxn to update LogTrack_config struct here */
    LogTrack_setTrackAddrs(trackAddr1, trackAddr2, trackAddr3, trackAddr4);

    /*
     * Send success to Host
     */
    httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_HTML);
    html(CRLF);

    return 1;
}

/*
 *  ======== cgiStop ========
 *  Stop streaming of log data.
 */
static int cgiStop(SOCKET htmlSock, int ContentLength, char *pArgs)
{

#ifdef LOGSERVER_DEBUG
    LOG_printf(&ti_bios_log_debugTrace, "LogServerCgi: cgiStop()\n");
#endif
    
    /* set global flag to stop streaming */
    _disable_interrupts();
    LogServerCgi_continueStreaming = FALSE;
    _enable_interrupts();

    /*
     * Send success to Host
     */
    httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_HTML);
    html(CRLF);

    return 1;
}

/*
 *  ======== cgiStreamLogs ========
 *  Read all log data for each log specified by logMask and send to host
 *  forever or until a stop command is received.
 */
static int cgiStreamLogs(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    Int parseIndex;
    int returnCode;
    Char *key;
    Char *dataBuffer;

    /* must use unsigned long type because I'm using strtoul() API */
    unsigned long scanmask;
    unsigned long pollFrequency;
    Int status;

#ifdef LOGSERVER_DEBUG
    LOG_printf( &ti_bios_log_debugTrace, "LogServerCgi: cgiStreamLogs");
#endif

    /* change this CGI task's priority to the user configured level */
    setCgiPriority();

    /* set global streaming flag to TRUE before beginning streaming */
    _disable_interrupts();
    LogServerCgi_continueStreaming = TRUE;
    _enable_interrupts();

    returnCode = 1;

#ifdef LOGSERVER_DEBUG
    printf("LogServerCgi: cgiStreamLogs: pArgs = %s\n", pArgs);
#endif

    /*
     * Process the CGI parameters.
     */
    parseIndex = 0;
    scanmask = 0;
    key = cgiParseVars( pArgs, &parseIndex );
    while (parseIndex != -1) {
        
#ifdef LOGSERVER_DEBUG
    printf("LogServerCgi: cgiStreamLogs: key = %s\n", key);
#endif

        if (!strcmp("logMask", key)) {
            key = cgiParseVars( pArgs, &parseIndex );

            scanmask = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            LOG_printf(&ti_bios_log_debugTrace, "LogServerCgi: cgiStreamLogs: scanmask = %x\n",
                    scanmask);
#endif

        }
        else if (!strcmp("pollRate", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

            pollFrequency = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: cgiStreamLogs: pollFrequency = %x\n",
                    pollFrequency);
#endif
        }
        else {

#ifdef LOGSERVER_DEBUG
            LOG_printf(&ti_bios_log_debugTrace,
    "ERROR: LogServerCgi: cgiStreamLogs: received unknown command from host\n");
#endif

            /* send error response */
            httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
            return 0;
        }
        key = cgiParseVars( pArgs, &parseIndex );
    }

    /* dynamically create storage for the log data buffer */
    dataBuffer = (Char *)MEM_alloc(LogServerCgi_config.segId,
            LogServerCgi_config.bufSize, 0);

    if (dataBuffer == MEM_ILLEGAL) {
        /*
         * No memory avaiable: send failure to Host
         */
        httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
        html(CRLF);
#ifdef LOGSERVER_DEBUG
            LOG_printf(&ti_bios_log_debugTrace,
                "ERROR: Capture Logs: Memory Allocation failure!\n");
#endif
        return 0;
    }
    else {
        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_APPLET);
        html(CRLF);
    }

    /*
     *      Receive data from log server.
     *
     *  For stream  mode, we will scan and send log data until a stop command
     *  has been issued.  After all the data from all the logs has been sent
     *  exactly once, this task will be put to sleep for the value specified by
     *  pollFrequency.  Each return from streamCaptureCmd() indicates that all
     *  logs have been fully scanned one time, and after each return the task
     *  is put to sleep.  This while loop will not end until the stopstream.cgi
     *  command is issued, or unless streamCaptureCmd() returns error status.
     */

    while (LogServerCgi_continueStreaming) {
        /* stream the log data */
        status = streamCaptureCmd(htmlSock, (Uint32)scanmask, dataBuffer);

        if (status < 0) {
            returnCode = 0;
            break;
        }

        /* only sleep if there was no stop cmd yet */
        if (LogServerCgi_continueStreaming) {
            /* sleep for time specified by polling frequency */
            TSK_sleep((Uns)pollFrequency);
        }
    }
    
    MEM_free(LogServerCgi_config.segId, dataBuffer,
            LogServerCgi_config.bufSize);

    return returnCode;
}


/*
 * ======== cgiWriteWord ========
 * Write a word of data to DSP memory.
 */
static int cgiWriteWord(SOCKET htmlSock, int ContentLength, char *pArgs)
{
    return (readWriteWordCmd(htmlSock, ContentLength, pArgs, WRITEWORD));
}

/*
 * ======== flushEnableDisableGetCpuCmd ========
 * Used for flush, enable, disable logs, and for getting the DSP speed.
 */
static int flushEnableDisableGetCpuCmd(SOCKET htmlSock, int ContentLength,
        char *pArgs, Int cmd)
{
    Char *key;
    Int parseIndex;
    Int sizeOfData;
    Int numBytesSent;
    Uint32 cpuSpeed;
    Uint32 *currDataAddr;

    /* must use unsigned long type because I'm using strtoul() API */
    unsigned long logmask;

#ifdef LOGSERVER_DEBUG
    LOG_printf( &ti_bios_log_debugTrace, "LogServerCgi: flushEnableDisableGetCpuCmd()");
#endif

    /* change this CGI task's priority to the user configured level */
    setCgiPriority();

#ifdef LOGSERVER_DEBUG
    LOG_printf( &ti_bios_log_debugTrace, "LogServerCgi: flushEnableDisableGetCpuCmd(): set pri = %d\n",            TSK_getpri((TSK_Handle)TaskSelf()) );
#endif

#ifdef LOGSERVER_DEBUG
    printf( "LogServerCgi: flushEnableDisableGetCpuCmd(): pArgs = %s, cmd = %d\n",
            pArgs, cmd);
#endif

    /*
     * Process the CGI parameters.
     */
    parseIndex = 0;
    logmask = 0;
    key = cgiParseVars( pArgs, &parseIndex );
    while ( parseIndex != -1 ) {
        
#ifdef LOGSERVER_DEBUG
    printf( "LogServerCgi: flushEnableDisableGetCpuCmd: key = %s\n", key);
#endif

        if( !strcmp("logMask", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: flushEnableDisableGetCpuCmd logging key = %s\n",
                    key);
#endif

            logmask = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("LogServerCgi: flushEnableDisableGetCpuCmd: logmask = %x\n",
                    logmask);
#endif

        }
        else {

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
          "ERROR: flushEnableDisableGetCpuCmd: received unknown argument from host.\n"
                );
#endif
            /* send error response */
            httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
            html(CRLF);
            
            return 0; 
        }
    }

    if (cmd == ENABLELOGS) {
        LogAux_enable(((Uint32)logmask));
    }
    else if (cmd == DISABLELOGS) {
        LogAux_disable(((Uint32)logmask));
    }
    else if (cmd == FLUSHLOGS) {
        LogAux_flush(((Uint32)logmask));
    }
    else if (cmd == GETCPUSPEED) {
        cpuSpeed = GBL_getFrequency();
    }
    else {
#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
          "ERROR: flushEnableDisableGetCpuCmd: received unknown command from host.\n"
                );
#endif
        /* send error response */
        httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
        html(CRLF);

        return 0; 
    }

    /* When getting cpu speed, must return this data to the Host using send() */
    if (cmd == GETCPUSPEED) {

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace, "LogServerCgi: cpu speed = %d\n",
                cpuSpeed);
#endif

        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_APPLET);
        html(CRLF);

        /*
         * Send the cpu speed back to Host.  Keep sending until
         * all data has been sent successfully.
         */
        sizeOfData = sizeof(Uint32);
        currDataAddr = &cpuSpeed;
        numBytesSent = 0;

        /* keep sending data until all data has been sent */
        while (sizeOfData > 0) {
            numBytesSent = send(htmlSock, currDataAddr, sizeOfData, 0);

            /* check return value of send() for error */
            if (numBytesSent != -1) {
                /* update the total amount of data left to send */ 
                sizeOfData -= numBytesSent;

                /* update curr pointer position by num of bytes already sent */
                currDataAddr += numBytesSent; 
            }
            else {
                /* determine the error code for the failure */
                int sendErrorCode = fdError();

                if (sendErrorCode == EWOULDBLOCK) {
                    /*
                     *  EWOULDBLOCK is not a true error; in this case,
                     *  just keep looping until all data is sent.
                     */
                    continue;
                }
#ifdef LOGSERVER_DEBUG
                LOG_printf(&ti_bios_log_debugTrace,
                        "LogServerCgi: flushEnableDisableGetCpuCmd: Error:");
                LOG_printf(&ti_bios_log_debugTrace, "\tsend returned -1, error code: %d",
                    sendErrorCode);
#endif

                /* exit the CGI command */
                return 0;
            }
        }
    }
    else {
        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_HTML);
        html(CRLF);
    }
    return 1;
}

/*
 * ======== readWriteWordCmd ========
 * Function called by cgiReadWord() and cgiWriteWord().  Used to read or write
 * a word of data to/from DSP memory.
 */
static int readWriteWordCmd(SOCKET htmlSock, int ContentLength, char *pArgs,
        Int cmd)
{
    Char *key;
    Int parseIndex;
    Int returnCode;
    Int sizeOfData;
    Int numBytesSent;

    /* must use unsigned long type because I'm using strtoul() API */
    unsigned long writeValueArg; /* used for writeword */
    unsigned long wordAddressArg;

    Uint32 *readWriteAddr;
    Uint32 *currDataAddr;
    Uint32 wordAddress;

#ifdef LOGSERVER_DEBUG
    printf( "readWriteWordCmd pArgs = %s\n", pArgs);
#endif

    /* change this CGI task's priority to the user configured level */
    setCgiPriority();

    /* initialize variables */
    returnCode = 1;
    parseIndex = 0;
    writeValueArg = 0;
    wordAddressArg = 0;
    sizeOfData = 0;
    numBytesSent = 0;
    
    /*
     * Process the CGI parameters.
     */
    while ( parseIndex != -1 ) {

        key = cgiParseVars( pArgs, &parseIndex );
#ifdef LOGSERVER_DEBUG
        printf( "readWriteWordCmd: key = %s\n", key);
#endif

        if( !strcmp("addr", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
            printf("readWriteWordCmd key = %s\n", key);
#endif

            wordAddressArg = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("readWriteWordCmd: addr = %x\n", wordAddressArg);
#endif

        }
        else if ( !strcmp("val", key) ) {
            key = cgiParseVars( pArgs, &parseIndex );

#ifdef LOGSERVER_DEBUG
            printf("cgiWriteWord key = %s\n", key);
#endif

            writeValueArg = strtoul(key, NULL, 16);

#ifdef LOGSERVER_DEBUG
            printf("cgiWriteWord: writeValueArg= %d\n", writeValueArg);
#endif

        }
        else {

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
            "ERROR: readWriteWordCmd: received unknown command from host.\n");
#endif
            /* send error response */
            httpSendStatusLine(htmlSock, HTTP_BAD_REQUEST, CONTENT_TYPE_HTML);
            html(CRLF);

            return 0;
        }
    }

    wordAddress = (Uint32)wordAddressArg;

    /* create a pointer from the address sent by the Host */
    readWriteAddr = (Uint32 *)wordAddress;

    if (cmd == WRITEWORD) {
        /* write data to the address specified by the Host */
        *readWriteAddr = (Uint32)writeValueArg;

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
            "LogServerCgi: word of data successfully written\n");
#endif
        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_HTML);
        html(CRLF);
        returnCode = 1;

    }
    else {
        /* read data from memory at the specified address */
#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace, "LogServerCgi: word of data read = %d\n",
                *readWriteAddr);
#endif

        /*
         * Send success to Host
         */
        httpSendStatusLine(htmlSock, HTTP_OK, CONTENT_TYPE_APPLET);
        html(CRLF);

        /*
         * Send word of data read from memory back to Host.  Keep sending until
         * all data has been sent successfully.
         */
        sizeOfData = sizeof(Uint32);
        currDataAddr = readWriteAddr;
        numBytesSent = 0;

        /* keep sending data until all data has been sent */
        while (sizeOfData > 0) {
            numBytesSent = send(htmlSock, currDataAddr, sizeOfData, 0);
            returnCode = 1;

            /* check return value of send() for error */
            if (numBytesSent != -1) {
                /* update the total amount of data left to send */ 
                sizeOfData -= numBytesSent;

                /* update curr pointer position by num of bytes already sent */
                currDataAddr += numBytesSent; 
            }
            else {
                /* determine the error code for the failure */
                int sendErrorCode = fdError();

                if (sendErrorCode == EWOULDBLOCK) {
                    /*
                     *  EWOULDBLOCK is not a true error; in this case,
                     *  just keep looping until all data is sent.
                     */
                    continue;
                }
#ifdef LOGSERVER_DEBUG
                LOG_printf(&ti_bios_log_debugTrace,
                        "LogServerCgi: readWriteWordCmd: Error:");
                LOG_printf(&ti_bios_log_debugTrace, "\tsend returned -1, error code: %d",
                    sendErrorCode);
#endif

                /* exit the CGI command */
                return 0;
            }
        }
    }
    
    return returnCode;
}

/*
 *  ======== setCgiPriority ========
 *  change the CGI thread priority to the value set by the user.
 */
static inline setCgiPriority(Void)
{
    Int cgiPriority;
    /* change this CGI task's priority to the user configured level */
    cgiPriority = LogServerCgi_config.priority;
    if ((cgiPriority < 1) || (cgiPriority > OS_TASKPRINORM)) {
        cgiPriority = OS_TASKPRINORM;
    }
    TSK_setpri(TSK_self(), cgiPriority);
}

/*
 *  ======== streamCaptureCmd ========
 *  This function contains common code used by cgiCaptureLogs and cgiStreamLogs.
 */
static Int streamCaptureCmd(SOCKET htmlSock, Uint32 scanmask,
        Char *dataBuffer)
{
    Int sizeOfLogData;
    Int numBytesSent;
    Int size;
    Char *logDataBuf;
    Int status;

    size = 0;
    sizeOfLogData = 0;
    numBytesSent = 0;

    /* "open" the log server for reading */
    LogAux_open(scanmask);

    do {

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
                "LogServerCgi: streamCaptureCmd: scanmask: %x", scanmask);
#endif

        /* initialize/reset size to be size of dataBuffer */
        size = LogServerCgi_config.bufSize;

        /* get the log data */
        status = LogAux_read(dataBuffer, &size);

        /* check the read status for error */
        if (status == -1) {
#ifdef DEBUG
            LOG_printf(&ti_bios_log_debugTrace,
                "LogServerCgi: streamCaptureCmd: Error: LogAux_read returned error status");
#endif
            break;
        }

#ifdef LOGSERVER_DEBUG
        LOG_printf(&ti_bios_log_debugTrace,
        "LogServerCgi: streamCaptureCmd: read data: status = %d\n\tsize = %d",
                status, size);
#endif

        /* 
         *     Send the log data to the host
         *
         *  Only send Host non empty buffers.  Keep sending until all log data
         *  in dataBuffer has been sent.  Free the reply message MSGQ and
         *  return upon failure.
         */
        if (size != 0) {
            sizeOfLogData = size;
            logDataBuf = dataBuffer;
            numBytesSent = 0;

            /* keep sending data until all data has been sent */
            while ( sizeOfLogData > 0) {

                numBytesSent =
                        send(htmlSock, logDataBuf, sizeOfLogData, 0);

                /* check return value of send() for error */
                if (numBytesSent != -1) {
                    /* update the total amount of log data left to send */ 
                    sizeOfLogData -= numBytesSent;

                    /* update curr buf position by num of bytes already sent */
                    logDataBuf += numBytesSent; 
                }
                else {
                    /* determine the error code for the failure */
                    int sendErrorCode = fdError();

                    if (sendErrorCode == EWOULDBLOCK) {
                        /*
                         *  EWOULDBLOCK is not a true error; in this case,
                         *  just keep looping until all data is sent.
                         */
                        continue;
                    }
#ifdef LOGSERVER_DEBUG
                    LOG_printf(&ti_bios_log_debugTrace,
                    "LogServerCgi: streamCaptureCmd: Error: send returned -1, error code: %d", sendErrorCode);
#endif
                    /* exit the CGI command */
                    status = -1;
                    break;
                }
            }
        }
    } while(status > 0);

    /* "close" the log server read */
    LogAux_close();
    return status;
}

/*
 */
/*
 *  @(#) ti.bios.log.ndk; 1, 0, 2, 02,31; 5-23-2009 17:29:07; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

