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
print "    Amount of memory given for DSP code and DSPLInk = ", $DSP_REGION_CODE_SIZE, "\n\n";
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
    s/^(IPC)=.*$/$1="$SELECTED_IPC"/g;
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

open OUTFILE, ">", $fileName or die "Could not open file. $!";
print OUTFILE ($string);
close OUTFILE;
print "done!\n";


# Modify the platform.tci memory variables
my $START_MAR               = hex($DSP_REGION_BASE_ADDR) >> 24;               # first MAR bit that should be on
my $END_MAR                 = ((hex($DSP_REGION_END_ADDR)+(1<<24)-1) >> 24);  # first MAR bit that should be off
my $START_FIRST_MAR_REGION  = ($START_MAR>>5)<<5;                             # Start MAR of first register that has some bits set
my $START_LAST_MAR_REGION   = ((($END_MAR-1)>>5)<<5)+31;                      # Start MAR of last register that has some bits set

# Figure out which MAR variables need to be modified and with what values
my @marString;
my @marValue;
for (my $MAR_start = $START_FIRST_MAR_REGION; $MAR_start<=$START_LAST_MAR_REGION; $MAR_start += 32)
{
  my $index           = ($MAR_start - $START_FIRST_MAR_REGION) >> 5;
  my $MAR_end         = $MAR_start+31;
  $marString[$index]  = "C64PLUSMAR" . $MAR_start . "to" . $MAR_end;
  if ($MAR_end < $END_MAR)
  {
    if ( $MAR_start > $START_MAR)
    {
      $marValue[$index] = sprintf "0x%08X", 0xFFFFFFFF;
    }
    else
    {
      $marValue[$index] = sprintf "0x%08X", 0xFFFFFFFF - ((1<<($START_MAR-$MAR_start)) - 1);
    }
  }
  else
  {
    if ( $MAR_start > $START_MAR)
    {
      $marValue[$index] = sprintf "0x%08X", ((1<<($END_MAR-$MAR_start)) - 1);
    }
    else
    {
      $marValue[$index] = sprintf "0x%08X", ((1<<($END_MAR-$MAR_start)) - 1) - ((1<<($START_MAR-$MAR_start)) - 1);
    }
    
  }
}

# Make the actual required edits in platform.tci
print "Updating platform platform.tci DSP/BIOS configuration...";
my $fileName = "$ENV{C6RUN_INSTALL_DIR}/platforms/$ENV{PLATFORM}/platform.tci";
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
  # Scan looking for correct MAR region(s)
  for (my $i = 0; $i<scalar(@marString); $i += 1)
  {
    if (/$marString[$i]/)
    {
      s/^(.*)($marString[$i]\s*)=.*$/$1$2= $marValue[$i]\;/g;
    }
  }
  $string = $string . $_;
}
close(INFILE);

open OUTFILE, ">", $fileName or die "Could not open file. $!";
print OUTFILE ($string);
close OUTFILE;
print "done!\n";



