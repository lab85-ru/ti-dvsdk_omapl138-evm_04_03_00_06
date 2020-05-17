#   ============================================================================
#   @file   Makefile
#
#   @path   $(DSPLINK)/gpp/src/
#
#   @desc   This file is a standard interface to the make scripts.
#           Usually no change is required in this file.
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
#   Include the configuration file
#   ============================================================================
include $(DSPLINK)$(DIRSEP)config$(DIRSEP)BUILD$(DIRSEP)CURRENTCFG.MK

#   ============================================================================
#   Check whether kbuild makefile or DSPLink generic build system is to be used
#   ============================================================================
ifeq ("$(TI_DSPLINK_GPPOS)", "Linux")
    ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL138")
        include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
        include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
        include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
    else # else ("$(TI_DSPLINK_PLATFORM)", "OMAPL138")
        ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")
            include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
            include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
            include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
        else # else ("$(TI_DSPLINK_PLATFORM)", "DA850")
            ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL1XX")
               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
            else # else ("$(TI_DSPLINK_PLATFORM)", "OMAPL1XX")
                ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
                   include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
                   include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
                   include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
                else # else ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
                    ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAP3530")
                       include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
                       include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
                       include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
                    else # else ("$(TI_DSPLINK_PLATFORM)", "OMAP3530")
		        ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCIHD")
                           include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
                           include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
                           include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
		        else # else ("$(TI_DSPLINK_PLATFORM)", "DAVINCIHD") 
    		            ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")
                               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_bld
                               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Rules.mk
                               include $(DSPLINK)$(DIRSEP)gpp$(DIRSEP)src$(DIRSEP)Makefile_kbuild
    		            else # else ("$(TI_DSPLINK_PLATFORM)", "DAVINCI") 
	                    include Makefile_bld
            	            endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")
            	        endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCIHD")
            	    endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAP3530")
                 endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
              endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL1XX")  
           endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")
        endif # ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL138")
else # else ("$(TI_DSPLINK_GPPOS)", "Linux")
    include Makefile_bld
endif # ifeq ("$(TI_DSPLINK_GPPOS)", "Linux")
