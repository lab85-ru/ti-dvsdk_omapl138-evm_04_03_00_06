#   ============================================================================
#   @file   c64xx.mk
#
#   @path   $(DSPLINK)/make/DspBios/
#
#   @desc   This makefile defines OS specific macros used by MAKE system for
#           the DSP/BIOS version 4.90.27 for C64XX.
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


ifndef C64XX_MK

define C64XX_MK
endef


#   ============================================================================
#   Let the make system know that a specific distribution for the GPP OS
#   is being used.
#   ============================================================================
USE_DISTRIBUTION := 1


#   ============================================================================
#   Set the values of necessary variables to be used for the OS.
#   ============================================================================

#   ----------------------------------------------------------------------------
#   Base directory for the DSP OS
#   ----------------------------------------------------------------------------
BASE_BUILDOS    := C:\ti\c6000\bios

#   ----------------------------------------------------------------------------
#   Base for code generation tools - compiler, linker, archiver etc.
#   ----------------------------------------------------------------------------
BASE_CGTOOLS    := C:\ti\c6000\cgtools
BASE_CGTOOLSBIN := $(BASE_CGTOOLS)\bin

#   ----------------------------------------------------------------------------
#   Base for TCONF utility and dependent components
#   ----------------------------------------------------------------------------
BASE_TCONF      := C:\ti\bin\utilities\tconf
BASE_PLATFORMS  := C:\ti\bin\utilities\tconf\include
BASE_RTDX       := C:\ti\c6000\rtdx
BASE_MSGQ       := C:\ti\messaging

#   ----------------------------------------------------------------------------
#   Base directory for include files provided by DSP OS
#   ----------------------------------------------------------------------------
BASE_OSINC      := $(BASE_BUILDOS)\include
BASE_CGTOOLSINC := $(BASE_CGTOOLS)\include
BASE_MSGQINC    := $(BASE_MSGQ)\include

OSINC_GENERIC   := $(BASE_OSINC) $(BASE_MSGQINC)
OSINC_PLATFORM  := $(BASE_CGTOOLSINC)

ifeq ($(DSPPLATFORM), morpheus_dev)
OSINC_PLATFORM  += C:\ti\boards\evmdm642\include
endif # ifeq ($(DSPPLATFORM), morpheus_dev)

ifeq ($(DSPPLATFORM), DM642)
OSINC_PLATFORM  += C:\ti\boards\evmdm642\include
endif # ifeq ($(DSPPLATFORM), DM642)

ifneq ("$(VARIANT)", "")
OSINC_VARIANT   := $(BASE_OSINC)
endif


#   ----------------------------------------------------------------------------
#   Base directory for libraries provided by DSP OS
#   ----------------------------------------------------------------------------
BASE_OSLIB      := $(BASE_BUILDOS)\lib
BASE_CGTOOLSLIB := $(BASE_CGTOOLS)\lib
BASE_RTDXLIB    := $(BASE_RTDX)\lib
BASE_CSLLIB     :=
BASE_PSLLIB     :=
BASE_MSGQLIB    := $(BASE_MSGQ)\lib

OSLIB_GENERIC   := $(BASE_OSLIB)
OSLIB_PLATFORM  := $(BASE_CGTOOLSLIB) $(BASE_RTDXLIB) \
                   $(BASE_CSLLIB) $(BASE_PSLLIB) $(BASE_MSGQLIB)

ifeq ($(DSPPLATFORM), morpheus_dev)
OSLIB_PLATFORM  += C:\ti\boards\evmdm642\lib
endif # ifeq ($(DSPPLATFORM), morpheus_dev)


ifeq ($(DSPPLATFORM), DM642)
OSLIB_PLATFORM  += C:\ti\boards\evmdm642\lib
endif # ifeq ($(DSPPLATFORM), DM642)

ifneq ("$(VARIANT)", "")
OSLIB_VARIANT   := $(BASE_OSLIB)
endif


#   ============================================================================
#   COMPILER
#   ============================================================================

#   ----------------------------------------------------------------------------
#   Name of the compiler
#   ----------------------------------------------------------------------------
COMPILER        := $(BASE_CGTOOLSBIN)\cl6x

#   ----------------------------------------------------------------------------
#   Command line switches used by the compiler
#
#   CC_SW_DEF       Command line defines
#   CC_SW_INC       Search path for header files
#   CC_SW_OBJ       Object file directory
#   CC_SW_DEB       Include debug information
#   CC_SW_REL       Release build
#   ----------------------------------------------------------------------------
CC_SW_DEF       := -d
CC_SW_INC       := -I
CC_SW_OBJ       := -fr
CC_SW_DEB       := -g -d"_DEBUG"
CC_SW_REL       := -o3

#   ----------------------------------------------------------------------------
#   Standard flags for the compiler
#   ----------------------------------------------------------------------------
STD_CC_FLAGS    := -q -pdr -pdv -pden -mv6400

#   ----------------------------------------------------------------------------
#   Standard flags for the compiler when building an executable
#   ----------------------------------------------------------------------------
EXE_CC_FLAGS    :=

#   ----------------------------------------------------------------------------
#   Flags for the compiler when building an archive
#   ----------------------------------------------------------------------------
ARC_CC_FLAGS    :=

#   ----------------------------------------------------------------------------
#   Standard definitions for the compiler
#   ----------------------------------------------------------------------------
STD_CC_DEFNS    :=


#   ============================================================================
#   ARCHIVER
#   ============================================================================
ARCHIVER        := $(BASE_CGTOOLSBIN)\ar6x

#   ----------------------------------------------------------------------------
#   Standard flags for the archiver
#   ----------------------------------------------------------------------------
STD_AR_FLAGS    := -r

#   ----------------------------------------------------------------------------
#   Archiver flags for extracting object files
#   ----------------------------------------------------------------------------
EXT_AR_FLAGS    := xq


#   ============================================================================
#   LINKER
#   ============================================================================
LINKER          := $(BASE_CGTOOLSBIN)\cl6x -z

#   ----------------------------------------------------------------------------
#   Command line switches used by the linker
#
#   LD_SW_INC       Search path for libraries
#   LD_SW_LIB       Include library name
#   LD_SW_OUT       Output file name
#   LD_SW_MAP       Map file name
#   LD_SW_RELOC     Generate relocateable output
#   ----------------------------------------------------------------------------
LD_SW_INC       := -i
LD_SW_LIB       := -l
LD_SW_OUT       := -o
LD_SW_MAP       := -m
LD_SW_RELOC     := -r

#   ----------------------------------------------------------------------------
#   Standard flags for the linker
#   ----------------------------------------------------------------------------
STD_LD_FLAGS    := -c -q -x

#   ----------------------------------------------------------------------------
#   Flags for the linker when building an executable
#   ----------------------------------------------------------------------------
EXE_LD_FLAGS    :=

ifeq ($(USE_MSGQ), 1)
EXE_LD_FLAGS    += $(LD_SW_LIB)"msgq.l64"  $(LD_SW_LIB)"mqtbiosque.l64"
endif # ifeq ($(USE_MSGQ), 1)


#   ============================================================================
#   TCONF
#   ============================================================================
TCONF           := $(BASE_TCONF)\tconf


#   ----------------------------------------------------------------------------
#   Standard flags for TCONF
#   ----------------------------------------------------------------------------
STD_TCF_FLAGS   :=


endif   # ifndef C64XX_MK
