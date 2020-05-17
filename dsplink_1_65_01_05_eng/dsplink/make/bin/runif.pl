#!/usr/bin/perl
#   ============================================================================
#   @file   runif.pl
#
#   @path   $(DSPLINK)/make/bin/
#
#   @desc   Perl script to conditionally execute a command.
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
#   cexec <condition> <command if true> <command if false>
#   ============================================================================

$debug = 0 ;                        # Set to 1 to print debug messages

#   ============================================================================
#   Define the TAGs used in configuration file
#   ============================================================================
my $TKN_EXIST    = 'exist'     ;

#  =============================================================================
#  Get the arguments
#  =============================================================================
if ($#ARGV < 0)
{
    die "USAGE : [PERL] runif.pl <platform> <condition> <command if true> <command if false>\n" ;
}
my $str_cmdline   = '' ;
my $str_condition = '' ;
my $str_cmdtrue   = '' ;
my $str_cmdfalse  = '' ;
my $cmdtoexecute  = 'echo Invalid arguments' ;

# - how to get the full command line
$str_condition = $ARGV [1] ;
$str_cmdtrue   = $ARGV [2] ;
$str_cmdfalse  = $ARGV [3] ;
#  =============================================================================
#  Parse the command
#  =============================================================================
#($str_condition, $str_cmdtrue, $str_cmdfalse) = split (/\s+/ , $str_cmdline) ;
if ($debug == 1)
{
    print "Condition    : $str_condition\n" ;
    print "Cmd If True  : $str_cmdtrue\n"   ;
    print "Cmd If False : $str_cmdfalse\n"  ;
}

#  =============================================================================
#  Check condition
#  =============================================================================
if ($str_condition =~ /^$TKN_EXIST\s+(.*)/)
{
    $filename = $1 ;
    if ( -e $filename )
    {
        $cmdtoexecute = $str_cmdtrue ;
    }
    else
    {
        $cmdtoexecute = $str_cmdfalse ;
    }
}

#  =============================================================================
#  Execute the conditional command
#  =============================================================================
if ($ARGV [0] eq "WINDOWS") {
    $retVal=system ($cmdtoexecute) ;
}
elsif ($ARGV [0] eq "LINUX") {
    $retVal=exec ($cmdtoexecute) ;
}

#  =============================================================================
#  DONE!!
#  =============================================================================
exit ($retVal) ;
