#!/usr/bin/perl
#   ============================================================================
#   @file   banner.pl
#
#   @path   $(DSPLINK)/make/bin/
#
#   @desc   Perl script to print formatted banners during execution of
#           make scripts.
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
#   banner.pl level component stage variant
#
#   level     = Level of print
#   component = Name of the component
#   stage     = Stage of build process [DIRS/INCLUDE/OBJECTS/...]
#   variant   = Build variant [DEBUG/RELEASE]
#
#   ============================================================================

$debug = 0 ;                        # Set to 1 to print debug messages

#  =============================================================================
#  Parse command line parameters
#  =============================================================================

$level      = $ARGV [0] ;   # Level
$component  = $ARGV [1] ;   # Module name
$stage      = $ARGV [2] ;   # Stage of build

if ($#ARGV >= 3)
{
    $variant = $ARGV [3] ;  # Build variant
}
else
{
    $variant = '' ;
}


if ($debug)
{
    print "level     : $level\n"  ;
    print "component : $component\n"  ;
    print "stage     : $stage\n"  ;
    print "variant   : $variant\n"  ;
}


#  =============================================================================
#  Print for style - BANNER
#  =============================================================================

$fmt_arg2   = "--------%-10s----------------------------------" ;
$fmt_arg3   = "--------%-10s--------------%-10s----------" ;

$fmt_l1     = "\n[%-20s] %s\n" ;
$fmt_l2     = "[%-20s] %s\n" ;

$outstr = '' ;
$outfmt = '' ;

if ($variant eq '')
{
    $outstr = sprintf "$fmt_arg2", $stage ;
}
else
{
    $outstr = sprintf "$fmt_arg3", $stage, $variant ;
}

$outstr =~ s/ /\-/g ;

$outstr =~ s/(\w)\-/$1 /g ;
$outstr =~ s/\-(\w)/ $1/g ;


if ($level == 1)
{
    $outstr =~ s/\-/\=/g ;

    $outfmt = $fmt_l1 ;
}
else
{
    $outfmt = $fmt_l2 ;
}

printf $outfmt, $component, $outstr ;


#  =============================================================================
#  DONE !!
#  =============================================================================
exit (0) ;
