#!/bin/perl
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
#     http://www.ti.com/                                                    #
#                                                                           #
#############################################################################
#
#############################################################################
#                                                                           #
#  Redistribution and use in source and binary forms, with or without       #
#  modification, are permitted provided that the following conditions       #
#  are met:                                                                 #
#                                                                           #
#    Redistributions of source code must retain the above copyright         #
#    notice, this list of conditions and the following disclaimer.          #
#                                                                           #
#    Redistributions in binary form must reproduce the above copyright      #
#    notice, this list of conditions and the following disclaimer in the    #
#    documentation and/or other materials provided with the                 #
#    distribution.                                                          #
#                                                                           #
#    Neither the name of Texas Instruments Incorporated nor the names of    #
#    its contributors may be used to endorse or promote products derived    #
#    from this software without specific prior written permission.          #
#                                                                           #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS      #
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT        #
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    #
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     #
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    #
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         #
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    #
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    #
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      #
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    #
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     #
#                                                                           #
#############################################################################

use strict;

my $DSP_REGION_BASE_ADDR = shift or die "No DSP_REGION_BASE_ADDR given.";
my $DSP_REGION_CMEM_SIZE = shift or die "No DSP_REGION_CMEM_SIZE given.";
my $DSP_REGION_CODE_SIZE = shift or die "No DSP_REGION_CODE_SIZE given.";

# Set DSP Region end address
my $DSP_REGION_END_ADDR = sprintf("0x%08X",hex($DSP_REGION_BASE_ADDR)+hex($DSP_REGION_CMEM_SIZE)+hex($DSP_REGION_CODE_SIZE));
my $string;

# Make the actual required edits in platform_syslink.bld
my $fileName = "$ENV{C6RUN_INSTALL_DIR}/platforms/$ENV{PLATFORM}/platform.bld";
if (-e $fileName)
{
  print "Updating platform's platform.bld SysBios/XDC build configuration...";
  open(INFILE, "<", $fileName) or die "Could not open file. $!";
  $string = "";
  while (<INFILE>)
  {
    if (/^(var\s+DSP_REGION_BASE_ADDR)\s*=.*$/)
    {
      s/^(var\s+DSP_REGION_BASE_ADDR)\s*=.*$/$1 = $DSP_REGION_BASE_ADDR\;/g;
    }
    if (/^(var\s+DSP_REGION_CMEM_SIZE)\s*=.*$/)
    {
      s/^(var\s+DSP_REGION_CMEM_SIZE)\s*=.*$/$1 = $DSP_REGION_CMEM_SIZE\;/g;
    }
    if (/^(var\s+DSP_REGION_CODE_SIZE)\s*=.*$/)
    {
      s/^(var\s+DSP_REGION_CODE_SIZE)\s*=.*$/$1 = $DSP_REGION_CODE_SIZE\;/g;
    }
    $string = $string . $_;
  }
  close(INFILE);
  
  open OUTFILE, ">", $fileName or die "Could not open file. $!";
  print OUTFILE ($string);
  close OUTFILE;
  print "done!\n";
}

