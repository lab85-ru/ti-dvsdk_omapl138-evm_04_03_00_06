#   ============================================================================
#   @file   start.mk
#
#   @path   $(DSPLINK)/make/
#
#   @desc   The makefile starts the process of building a component.
#           It defines all the commonly used targets for the 'make' system.
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


# Check for MAKE version
$(if $(filter $(MAKE_VERSION), 3.81 3.81beta1 3.90 3.92),,\
$(error This makefile requires one of GNU make version 3.81 3.81beta1 3.90 3.92))
#   ============================================================================
#   Export the directory separator
#   ============================================================================
export DIRSEP

include $(DSPLINK)$(DIRSEP)config$(DIRSEP)BUILD$(DIRSEP)CURRENTCFG.MK

#   ============================================================================
#   Set the root for this 'make' system
#   ============================================================================
export MAKEROOT := $(DSPLINK)$(DIRSEP)make

include $(MAKEROOT)$(DIRSEP)processor.mk

#   ----------------------------------------------------------------------------
#   Include the appropriate DIRS file (if it exists).
#   It contains list of sub-directories to be considered for build.
#   ----------------------------------------------------------------------------

-include .$(DIRSEP)DIRS


#   ============================================================================
#   Select the appropriate make file to set things rolling...
#   ============================================================================
ifeq ("$(DIRS)", "")

#   ----------------------------------------------------------------------------
#   Include the appropriate COMPONENT file based on device type.
#   It contains necessary information to build the component.
#   ----------------------------------------------------------------------------
-include .$(DIRSEP)$(BUILDOS)$(DIRSEP)COMPONENT
-include .$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_GPPOSVERSION)$(DIRSEP)COMPONENT

export COMP_NAME
export COMP_PATH

-include $(MAKEROOT)$(DIRSEP)current.mk

else

-include $(MAKEROOT)$(DIRSEP)recurse.mk

endif
