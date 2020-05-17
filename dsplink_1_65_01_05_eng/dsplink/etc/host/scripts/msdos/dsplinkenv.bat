@echo off
rem ============================================================================
rem @file   dsplinkenv.bat
rem
rem @path   $(DSPLINK)/etc/host/scripts/msdos/
rem
rem @desc   Sets the necessary environment for DSP/BIOS(TM) Link development.
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


rem ----------------------------------------------------------------------------
rem @name   DSPLINK
rem
rem @desc   Root directory of DSP/BIOS LINK.
rem
rem Note: Use the current directory path and remove the "\etc\host\scripts\msdos"
rem       substring from it.
rem         
rem ----------------------------------------------------------------------------
set CURR_PATH=%CD%
set DSPLINK=%CURR_PATH:\etc\host\scripts\msdos=%

rem ----------------------------------------------------------------------------
rem @name   PATH
rem
rem @desc   Appends the path to the host scripts
rem ----------------------------------------------------------------------------

set PERL_BIN_PATH=C:\Perl\bin

if not [%BASE_PERL%]==[] set PERL_BIN_PATH=%BASE_PERL%\bin

set PATH=%PATH%;%DSPLINK%\etc\host\scripts\msdos;%PERL_BIN_PATH%


echo ===========================================================
echo The environment for DSP/BIOS LINK development has been set:
echo DSPLINK  = %DSPLINK%
echo PATH    += %DSPLINK%\etc\host\scripts\msdos;%PERL_BIN_PATH%
echo ===========================================================



set PERL_BIN_PATH=



