#!/usr/bin/perl
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

my $list_file = shift or die "Function list filename not specified on the command line.\n";
my $symtab_file = shift or die "Symbol table input filename not specified on the command line.\n";
my $out_file = shift or die "Output filename not specified on the command line.\n";

# Open the output file and input c file for writing
open(listFile, '<', $list_file) or die $!;
open(symtabFile, '<', $symtab_file) or die $!;
open(outFile, '>',  $out_file) or die $!;

# Get list of function names
my @fxnNames;
@fxnNames = <listFile>;
close(listFile);

my $fxnName;

print outFile "#include <stdint.h>\n\n";
while(<symtabFile>)
{
  foreach $fxnName (@fxnNames)
  {
    chomp($fxnName);
    
    my $fxnString = "_$fxnName" . "_stub";
    if (/\s+$fxnString\s*$/)
    {
      if (/\s+([0-9a-fA-F]{8})\s+/)
      {
        print outFile "intptr_t FXN_ADDR_",$fxnName,"_stub = 0x$1;\n";
      }
    }
  }
}
print outFile "\n";

# Clean up all opened files
close(outFile);
close(symtabFile);
close(listFile);
