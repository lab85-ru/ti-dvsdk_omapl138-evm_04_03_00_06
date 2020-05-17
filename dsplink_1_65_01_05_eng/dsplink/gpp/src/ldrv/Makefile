#   ============================================================================
#   @file   Makefile
#
#   @path   $(DSPLINK)/gpp/src/ldrv/
#
#   @desc   This file is a standard interface to the make scripts.
#           Usually no change is required in this file.
#
#   @ver    1.65.01.05_eng
#   ============================================================================
#   Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation version 2.
#   
#   This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
#   whether express or implied; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#   ============================================================================


#   ============================================================================
#   Set the device type (GPP/DSP)
#   ============================================================================

export DEVICETYPE := GPP


#   ============================================================================
#   Get the directory separator used on the development host.
#   ============================================================================

ifneq ("$(ComSpec)", "")
    ifeq ("$(OSTYPE)", "cygwin")
        DIRSEP ?=/
    else
        DIRSEP ?=\\
    endif
else
    DIRSEP ?= /
endif


#   ============================================================================
#   Start the build process
#   ============================================================================

include $(DSPLINK)$(DIRSEP)make$(DIRSEP)start.mk
