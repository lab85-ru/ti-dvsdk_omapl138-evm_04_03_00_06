#   ============================================================================
#   @file   rulebase.mk
#
#   @path   $(DSPLINK)/make/
#
#   @desc   Defines rules for all make targetes exported by the system.
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


ifndef RULEBASE_MK

define RULEBASE_MK
endef

#   ============================================================================
#   @rule   all
#
#   @desc   Make all targets and export them
#   ============================================================================
all    : debug release

#   ============================================================================
#   @rule   clean
#
#   @desc   Clean the intermediate files generated during the make process
#   ============================================================================
clean   : cleandeb cleanrel

#   ============================================================================
#   @rule objects
#
#   @desc Make the objects from specified sources
#   ============================================================================
objects : objectsdeb objectsrel

#   ============================================================================
#   @rule objects
#
#   @desc Make the objects from DEBUG sources
#   ============================================================================
objectsdeb : export BUILD_MODE := DEB
objectsdeb : objdeb

#   ============================================================================
#   @rule objects
#
#   @desc Make the objects from RELEASE sources
#   ============================================================================
objectsrel : export BUILD_MODE := REL
objectsrel : objrel

#   ============================================================================
#   @rule   debug
#
#   @desc   Make all targets in DEBUG mode and export them
#   ============================================================================
debug   : export BUILD_MODE := DEB
debug   : preprocess dirinc dirdeb direxp dsplinkdir bldinfodeb cleandeb includes objdeb trgdeb expdeb

#   ============================================================================
#   @rule   release
#
#   @desc   Make all targets in RELEASE mode and export them
#   ============================================================================
release : export BUILD_MODE := REL
release : preprocess dirinc dirrel direxp dsplinkdir bldinforel cleanrel includes objrel trgrel exprel

#   ============================================================================
#   @rule   bldinfo
#
#   @desc   Generate build information used
#   ============================================================================
bldinfo : preprocess dirs clean includes dsplinkdir bldinfodeb bldinforel

#   ============================================================================
#   @rule   preprocess
#
#   @desc   Perform the necessary pre-build steps.
#   ============================================================================
preprocess :

endif   # ifndef RULEBASE_MK
