#   ============================================================================
#   @file   linuxpc2.6.mk
#
#   @path   $(DSPLINK)/make/Linux/
#
#   @desc   This makefile defines OS specific macros used by MAKE system.
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


ifndef LINUX_2_6_MK

define LINUX_2_6_MK
endef


#   ============================================================================
#   Let the make system know that a specific distribution for the GPP OS
#   is being used.
#   ============================================================================
USE_DISTRIBUTION := 1


#   ============================================================================
#   Base directory for the GPP OS
#   ============================================================================
BASE_BUILDOS    := /usr/src/kernels/linux-2.6.21/

#   ============================================================================
#   Base toolchain for code generation tools - compiler, linker, archiver etc.
#   ============================================================================
BASE_TOOLCHAIN  := /usr

#   ============================================================================
#   Base for code generation tools - compiler, linker, archiver etc.
#   ============================================================================
BASE_CGTOOLS    := $(BASE_TOOLCHAIN)/bin


#   ============================================================================
#   Base directory for include files provided by GPP OS
#   ============================================================================
BASE_OSINC      := $(BASE_BUILDOS)/include

OSINC_GENERIC   := $(BASE_OSINC)
OSINC_PLATFORM  := $(BASE_OSINC)/asm/mach-default
OSINC_TARGET    := 


#   ============================================================================
#   Base directory for libraries provided by GPP OS
#   ============================================================================
BASE_OSLIB      := /usr/lib

OSLIB_GENERIC   := $(BASE_OSLIB)
OSLIB_PLATFORM  := $(BASE_OSLIB)


#   ============================================================================
#   COMPILER & Defines/flags
#   ============================================================================
COMPILER    	:= gcc

#   ----------------------------------------------------------------------------
#   Command line switches used by the compiler
#
#   CC_SW_DEF       Command line defines
#   CC_SW_INC       Search path for header files
#   CC_SW_OBJ       Create object file
#   CC_SW_DEB       Include debug information
#   ----------------------------------------------------------------------------
CC_SW_DEF       := -D
CC_SW_INC       := -I
CC_SW_OBJ       := -o
CC_SW_DEB       := -g

#   ----------------------------------------------------------------------------
#   Standard flags for the compiler
#   ----------------------------------------------------------------------------
STD_KRNL_FLAGS  :=   -Wall -c -nostdinc -iwithprefix include -D__KERNEL__      \
					 -Iinclude  -Wno-trigraphs -fno-strict-aliasing -fno-common\
					 -Os -fomit-frame-pointer                                  \
					 -pipe -msoft-float -m32 -fno-builtin-sprintf              \
					 -fno-builtin-log2 -fno-builtin-puts                       \
					 -mpreferred-stack-boundary=2                              \
					 -march=i686 -mregparm=3
STD_USER_FLAGS  := -Wall -c -fno-strict-aliasing

#   ----------------------------------------------------------------------------
#   Flags for the compiler when building a library
#   ----------------------------------------------------------------------------
LIB_CC_FLAGS    	:= -D_REENTRANT

#   ----------------------------------------------------------------------------
#   Standard flags for the compiler when building an executable
#   ----------------------------------------------------------------------------
EXE_CC_FLAGS    	:= -O

#   ----------------------------------------------------------------------------
#   Flags for the compiler when building a driver
#   ----------------------------------------------------------------------------
DRV_CC_FLAGS    	:= -nostdinc -O -D_REENTRANT

#   ----------------------------------------------------------------------------
#   Standard definitions for the compiler
#   ----------------------------------------------------------------------------
STD_CC_DEFNS    := 


#   ============================================================================
#   LINKER
#   ============================================================================
LD      	:= ld

#   ----------------------------------------------------------------------------
#   Command line switches used by the linker
#
#   LD_SW_LIB       Search path for libraries
#   LD_SW_OUT       Output filename
#   LD_SW_RELOC     Generate relocateable output
#   ----------------------------------------------------------------------------
LD_SW_LIB       := -L
LD_SW_OUT       := -o
LD_SW_RELOC     := -r

#   ----------------------------------------------------------------------------
#   Flags for the linker when building an executable
#   ----------------------------------------------------------------------------
EXE_LD_FLAGS      := -m elf_i386 -lpthread -lc 

#   ----------------------------------------------------------------------------
#   Flags for the linker when building a driver
#   ----------------------------------------------------------------------------
DRV_LD_FLAGS    	:= -nostartfiles -nodefaultlibs

#   ----------------------------------------------------------------------------
#   Standard flags for the linker
#   ----------------------------------------------------------------------------
STD_LD_FLAGS    := 


#   ============================================================================
#   ARCHIVER
#   ============================================================================
ARCHIVER        := ar

#   ----------------------------------------------------------------------------
#   Standard flags for the archiver
#   ----------------------------------------------------------------------------
STD_AR_FLAGS    := rs


#   ============================================================================
#   LINKER used for building execution
#   ============================================================================
LINKER     	:= gcc


#   ============================================================================
#   Post processing utilities for Linux 2.6
#   ============================================================================
CMD_MODPOST     := $(BASE_BUILDOS)/scripts/mod/modpost -i $(BASE_BUILDOS)/Module.symvers

endif   # ifndef LINUX_2_6_MK
