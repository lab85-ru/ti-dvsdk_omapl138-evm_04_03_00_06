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

my $SELECTED_IPC         = shift or die "No IPC given.";
my $DSP_REGION_BASE_ADDR = shift or die "No DSP_REGION_BASE_ADDR given.";
my $DSP_REGION_CMEM_SIZE = shift or die "No DSP_REGION_CMEM_SIZE given.";
my $DSP_REGION_CODE_SIZE = shift or die "No DSP_REGION_CODE_SIZE given.";

# Set DSP Region end address
my $DSP_REGION_END_ADDR = sprintf("0x%08X",hex($DSP_REGION_BASE_ADDR)+hex($DSP_REGION_CMEM_SIZE)+hex($DSP_REGION_CODE_SIZE));

print "Using the following settings for the platform memory map:\n";
print "    Selected software mechanism                     = ", $SELECTED_IPC, "\n";
print "    Base Address of Memory Given for DSP Usage      = ", $DSP_REGION_BASE_ADDR, "\n";
print "    Amount of memory given for shared heap memory   = ", $DSP_REGION_CMEM_SIZE, "\n";
print "    Amount of memory given for DSP code and ", $SELECTED_IPC, " = ", $DSP_REGION_CODE_SIZE, "\n\n";
print "It is the user's responsibility to ensure that the physical memory region from\n";
print "$DSP_REGION_BASE_ADDR to $DSP_REGION_END_ADDR is not used by the ARM/Linux OS.\n\n";

my $string;

# Modify the loadmodules.sh script
print "Updating platform loadmodules script...";
my $fileName = "$ENV{C6RUN_INSTALL_DIR}/platforms/$ENV{PLATFORM}/loadmodules.sh";
open(INFILE, "<", $fileName) or die "Could not open file. $!";
$string = "";
while (<INFILE>)
{
  if (/^(IPC)=.*$/)
  {
    s/^(IPC)=.*$/$1=$SELECTED_IPC/g;
  }
  if (/^(DSP_REGION_END_ADDR)=.*$/)
  {
    s/^(DSP_REGION_END_ADDR)=.*$/$1="$DSP_REGION_END_ADDR"/g;
  }
  if (/^(DSP_REGION_START_ADDR)=.*$/)
  {
    s/^(DSP_REGION_START_ADDR)=.*$/$1="$DSP_REGION_BASE_ADDR"/g;
  }
  $string = $string . $_;
}
close(INFILE);

open(OUTFILE, ">", $fileName) or die "Could not open file. $!";
print OUTFILE ($string);
close OUTFILE;
print "done!\n";


# Modify the unloadmodules.sh script
print "Updating platform unloadmodules script...";
my $fileName = "$ENV{C6RUN_INSTALL_DIR}/platforms/$ENV{PLATFORM}/unloadmodules.sh";
open(INFILE, "<", $fileName) or die "Could not open file. $!";
$string = "";
while (<INFILE>)
{
  if (/^(IPC)=.*$/)
  {
    s/^(IPC)=.*$/$1=$SELECTED_IPC/g;
  }
  $string = $string . $_;
}
close(INFILE);

open(OUTFILE, ">", $fileName) or die "Could not open file. $!";
print OUTFILE ($string);
close OUTFILE;
print "done!\n";



