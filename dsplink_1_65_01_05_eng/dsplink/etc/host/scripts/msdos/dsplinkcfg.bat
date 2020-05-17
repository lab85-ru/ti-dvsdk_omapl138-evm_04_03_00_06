@echo off
rem ============================================================================
rem @file   dsplinkcfg.bat
rem
rem @path   $(DSPLINK)/etc/host/scripts/msdos/
rem
rem @desc   Calls perl script to create build configuration for
rem         DSP/BIOS(TM) Link
rem
rem @ver    1.65.01.05_eng
rem ============================================================================
rem Copyright (C) 2002-2009, Texas Instruments Incorporated -
rem http://www.ti.com/
rem
rem Redistribution and use in source and binary forms, with or without
rem modification, are permitted provided that the following conditions
rem are met:
rem 
rem *  Redistributions of source code must retain the above copyright
rem    notice, this list of conditions and the following disclaimer.
rem 
rem *  Redistributions in binary form must reproduce the above copyright
rem    notice, this list of conditions and the following disclaimer in the
rem    documentation and/or other materials provided with the distribution.
rem 
rem *  Neither the name of Texas Instruments Incorporated nor the names of
rem    its contributors may be used to endorse or promote products derived
rem    from this software without specific prior written permission.
rem 
rem THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
rem AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
rem THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
rem PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
rem CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
rem EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
rem PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
rem OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
rem WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
rem OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
rem EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
rem ============================================================================

pushd %DSPLINK%\config\bin
perl dsplinkcfg.pl %*
popd
