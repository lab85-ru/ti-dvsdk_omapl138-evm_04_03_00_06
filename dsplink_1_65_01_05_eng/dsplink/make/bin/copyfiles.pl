#!/usr/bin/perl
#   ============================================================================
#   @file   copyfiles.pl
#
#   @path   $(DSPLINK)/make/bin/
#
#   @desc   Perl script to copy files to export directory
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

#   ============================================================================
#   USAGE:
#   copy <source> <destination>
#   ============================================================================

#  =============================================================================
#  Get the arguments
#  =============================================================================
if ($#ARGV < 0)
{
    die "USAGE : [PERL] copyfiles.pl <source> <destination>\n" ;
}

my $str_source    = '' ;
my $str_destination  = '' ;
my $cmdtoexecute  = 'echo Invalid arguments' ;

# - how to get the full command line
$str_source      = $ARGV [1] ;
$str_destination = $ARGV [2] ;

#  =============================================================================
#  Execute the conditional command
#  =============================================================================
# This file copies all include files from $dsplink/[gpp/dsp]/inc to the export
# directory. It then removes all internal header files.
if ($ARGV [0] eq "WINDOWS") {
    $cmdtoexecute="copy " . $str_source . "\\*.h " . $str_destination ;
    $retVal=system ($cmdtoexecute) ;
}
elsif ($ARGV [0] eq "LINUX") {
    $cmdtoexecute="cp -f " . $str_source . "/*.h " . $str_destination ;
    $retVal=system ($cmdtoexecute) ;
}

#  =============================================================================
#  DONE!!
#  =============================================================================
exit ($retVal) ;
