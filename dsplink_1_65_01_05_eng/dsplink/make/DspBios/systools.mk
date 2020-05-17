#   ============================================================================
#   @file   systools.mk
#
#   @path   $(DSPLINK)/make/DspBios/
#
#   @desc   This makefile defines all system calls and tools for the build OS.
#           It supports build on Linux & Windows OSes.
#
#   @ver    1.65.01.05_eng
#   ============================================================================
#   Copyright (C) 2002-2009, Texas Instruments Incorporated -
#   http://www.ti.com/
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   ============================================================================


ifndef SYSTOOLS_MK

define SYSTOOLS_MK
endef


#   ============================================================================
#   Check if running on Windows.
#   ============================================================================
ifneq ("$(ComSpec)", "")


#   ----------------------------------------------------------------------------
#   Base directory for the tools
#   ----------------------------------------------------------------------------
BASE_TOOLS := C:


#   ----------------------------------------------------------------------------
#   Base directory for other tools
#   ----------------------------------------------------------------------------
ifeq ("$(BASE_PERL)", "")
BASE_PERL := C:\perl
endif


#   ----------------------------------------------------------------------------
#   File for redirecting unwanted output
#   ----------------------------------------------------------------------------
TRASH := $(TEMP)\null


#   ----------------------------------------------------------------------------
#   Define macros to handle VERBOSE option
#
#   CMDHIDE     : Token to prevent a command from echoing on the console
#   CHMECHO     : Token to allow/prevent a command to echoing on the console
#                 This macro is controlled by value of VERBOSE
#   ----------------------------------------------------------------------------

CMDHIDE := @

ifneq ("$(VERBOSE)", "1")
VERBOSE := 0
endif

ifeq ("$(VERBOSE)", "1")
CMDECHO :=
else
CMDECHO := @
endif

#   ----------------------------------------------------------------------------
#   Define macros for common shell commands
#
#   DISPLAY     : Display on screen
#   SETENV      : Set environment variable
#   MKDIR       : Make a directory
#   RMDIR       : Remove a directory
#   CHDIR       : Change to a directory
#   COPY        : Copy a file
#   XCOPY       : Copy files and directories
#   DELETE      : delete a file
#   RMTREE      : Remove a directory tree
#   IFF         : conditional execution
#   APPENDTO    : Shell token representing 'append' operator
#   SENDTO      : Shell token representing 'redirect' operator
#   ----------------------------------------------------------------------------

DISPLAY  := echo
SETENV   := set
MKDIR    := mkdir
RMDIR    := rmdir /q
CHDIR    := cd
COPY     := copy
XCOPY    := xcopy
DELETE   := del /q
RMTREE   := rmdir /q /s
IFF      := if
APPENDTO := >>
SENDTO   := >


#   ----------------------------------------------------------------------------
#   Define macros for other tools
#
#   BANNER      : Display banner on screen
#   ----------------------------------------------------------------------------

PERL      := $(BASE_PERL)$(DIRSEP)bin$(DIRSEP)perl

BANNER    := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)banner.pl

RUNIF     := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)runif.pl WINDOWS

COPYFILES := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)copyfiles.pl WINDOWS


else   #ifneq ("$(ComSpec)", "")


#   ----------------------------------------------------------------------------
#   Base directory for the tools
#   ----------------------------------------------------------------------------
BASE_TOOLS := /usr/bin


#   ----------------------------------------------------------------------------
#   Base directory for other tools
#   ----------------------------------------------------------------------------
BASE_PERL := /usr/


#   ----------------------------------------------------------------------------
#   File for redirecting unwanted output
#   ----------------------------------------------------------------------------
TRASH := /dev/null


#   ----------------------------------------------------------------------------
#   Define macros to handle VERBOSE option
#
#   CMDHIDE     : Token to prevent a command from echoing on the console
#   CHMECHO     : Token to allow/prevent a command to echoing on the console
#                 This macro is controlled by value of VERBOSE
#   ----------------------------------------------------------------------------

CMDHIDE := @

ifneq ("$(VERBOSE)", "1")
VERBOSE := 0
endif

ifeq ("$(VERBOSE)", "1")
CMDECHO :=
else
CMDECHO :=
endif

#   ----------------------------------------------------------------------------
#   Define macros for common shell commands
#
#   DISPLAY     : Display on screen
#   SETENV      : Set environment variable
#   MKDIR       : Make a directory
#   RMDIR       : Remove a directory
#   CHDIR       : Change to a directory
#   COPY        : Copy a file
#   XCOPY       : Copy files and directories
#   DELETE      : delete a file
#   RMTREE      : Remove a directory tree
#   IFF         : conditional execution
#   APPENDTO    : Shell token representing 'append' operator
#   SENDTO      : Shell token representing 'redirect' operator
#   ----------------------------------------------------------------------------

DISPLAY  := echo
SETENV   := set
MKDIR    := mkdir -p
RMDIR    := rmdir
CHDIR    := cd
COPY     := cp
XCOPY    := xcopy
DELETE   := rm -f
RMTREE   := rm -r -f
IFF      := if
APPENDTO := >>
SENDTO   := >


#   ----------------------------------------------------------------------------
#   Define macros for other tools
#
#   BANNER      : Display banner on screen
#   ----------------------------------------------------------------------------

PERL      := $(BASE_PERL)$(DIRSEP)bin$(DIRSEP)perl

BANNER    := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)banner.pl

RUNIF     := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)runif.pl LINUX

COPYFILES := $(PERL) $(MAKEROOT)$(DIRSEP)bin$(DIRSEP)copyfiles.pl LINUX


endif   #ifneq ("$(ComSpec)", "")

endif   # ifndef SYSTOOLS_MK
