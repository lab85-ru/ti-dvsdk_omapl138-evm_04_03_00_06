/*
 * liddSample_io.c
 *
 * This file contains the test / demo code to demonstrate the LCDC LIDD driver 
 * functinality on DSP/BIOS 5. This demonstration is based on a character LCD
 * display.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** \file   liddSample_io.c
 *
 *   \brief  This file contains sample code to demonstrate the
 *          LIDD driver implemented for DSP/BIOS - EVM 6747
 *
 * (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PSG
 *  \version    0.1
 */
#include <std.h>
#include <stdio.h>
#include <string.h>

#include <tsk.h>
#include <log.h>
#include <iom.h>
#include <gio.h>

#include <ti/pspiom/lcdclidd/Lidd.h>

/*
 * External references
 */
extern LOG_Obj    trace;
extern GIO_Handle liddChanHandle;

static Char *welcome = "        WELCOME         " ;
static Char *demo    = "This is LIDD DRIVER demo" ;


/**
 * The main sample application of LIDD
 */
Int32 runLiddSample(Void)
{
    Int32                   retVal = IOM_COMPLETED;
    Lidd_DataParam          dataParam;
    Lidd_IoctlCmdArg        ioctlArg;
    Lidd_CursorPosition     cursorPos;
    Uint8                   i;

    LOG_printf(&trace, "\nStarting LIDD Sample Application");

    LOG_printf(&trace, "\n A sample text is displayed on the character LCD panel");

    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_DISPLAY_ON, &ioctlArg);

    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_CURSOR_ON, &ioctlArg);

    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_BLINK_ON, &ioctlArg);


    dataParam.buffer = (Uint8*)welcome;
    dataParam.len = 24;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 24;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Scroll Display");
    LOG_printf(&trace, "\n");

    for(i=0; i<24;i++)
    {
        ioctlArg.val = 0; /* Dont care */
        ioctlArg.chipSel = 0;
        GIO_control(liddChanHandle,
                Lidd_IOCTL_DISPLAY_MOVE_RIGHT,
                &ioctlArg);
        TSK_sleep(200);
    }

    TSK_sleep(2000);

    for(i=0; i<24;i++)
    {
        ioctlArg.val = 0; /* Dont care */
        ioctlArg.chipSel = 0;
        GIO_control(liddChanHandle,
                       Lidd_IOCTL_DISPLAY_MOVE_LEFT,
                       &ioctlArg);
        TSK_sleep(200);
    }

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Cursor Home");
    LOG_printf(&trace, "\n");


    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle,
                   Lidd_IOCTL_CURSOR_HOME,
                   &ioctlArg);
    TSK_sleep(1000);


    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Move cursor");
    LOG_printf(&trace, "\n");

    for(i=0; i<24;i++)
    {
        ioctlArg.val = 0; /* Dont care */
        ioctlArg.chipSel = 0;
        GIO_control(liddChanHandle,
                       Lidd_IOCTL_CURSOR_MOVE_RIGHT,
                       &ioctlArg);
        TSK_sleep(200);
    }

    TSK_sleep(2000);

    for(i=0; i<24;i++)
    {
        ioctlArg.val = 0; /* Dont care */
        ioctlArg.chipSel = 0;
        GIO_control(liddChanHandle,
                       Lidd_IOCTL_CURSOR_MOVE_LEFT,
                       &ioctlArg);
        TSK_sleep(200);
    }

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Set cursor Position");
    LOG_printf(&trace, "\n");


    cursorPos.col  = 8; /* Dont care */
    cursorPos.line = 1;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = '(';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    cursorPos.col  = 16; /* Dont care */
    cursorPos.line = 1;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = ')';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);


    cursorPos.col  = 5; /* Dont care */
    cursorPos.line = 2;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = '*';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    cursorPos.col  = 8; /* Dont care */
    cursorPos.line = 2;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = '*';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    cursorPos.col  = 13; /* Dont care */
    cursorPos.line = 2;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = '*';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    cursorPos.col  = 20; /* Dont care */
    cursorPos.line = 2;
    cursorPos.chipSel = 0;

    GIO_control(liddChanHandle,
                   Lidd_IOCTL_SET_CURSOR_POSITION,
                   &cursorPos);

    TSK_sleep(1000);

    *demo = '*';
    dataParam.buffer = (Uint8*)demo;
    dataParam.len = 1;
    dataParam.chipSel = 0;
    GIO_write(liddChanHandle, &dataParam, &dataParam.len);

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Cursor Blink Off");
    LOG_printf(&trace, "\n");
        
    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_BLINK_OFF, &ioctlArg);

    TSK_sleep(5000);

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Cursor Off");
    LOG_printf(&trace, "\n");
        
    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_CURSOR_OFF, &ioctlArg);

    TSK_sleep(5000);

    LOG_printf(&trace, "\n");
    LOG_printf(&trace, "Display Off");
    LOG_printf(&trace, "\n");
        
    ioctlArg.val = 0; /* Dont care */
    ioctlArg.chipSel = 0;
    GIO_control(liddChanHandle, Lidd_IOCTL_SET_DISPLAY_OFF, &ioctlArg);

    TSK_sleep(5000);

    GIO_delete(liddChanHandle);

    LOG_printf(&trace, "\nLIDD Sample Application Ends");

    return retVal;
}

/*!
*! Revision History
*! ================
*! 22-Dec-2008 connell created from IDriver example.
*! */


