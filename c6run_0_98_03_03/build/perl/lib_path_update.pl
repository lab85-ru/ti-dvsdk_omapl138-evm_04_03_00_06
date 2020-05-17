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
use File::Spec;

# Select Debug output
my $debug = 0;

my $LIBLIST_LINK_TXT_FILE   = shift or die "No input library list text file given.";
my $LIBLIST_SEARCH_PATH     = shift or die "No library search path given.";

# Step 0 - Make sure input file exists
if (! -e $LIBLIST_LINK_TXT_FILE)
{
  die "File $LIBLIST_LINK_TXT_FILE does not exit!";
}


# Step 1
#   Read in the text file containing the list of libraries derived from linker command file
open(INFILE, "<", $LIBLIST_LINK_TXT_FILE) or die "Could not open file. $!";
my @liblist = <INFILE>;
close(INFILE);

  
# Step 2
#   Search in specified search path for files of the same name as those in the original input
#   list.  Create an output string consisting of modified paths (when file found) and original
#   names when file is not found
my $outputString = "";
for (my $lineNum=0; $lineNum<=$#liblist; $lineNum++)
{
  $_ = @liblist[$lineNum];
  chomp;
  s/^\s+//;
	s/\s+$//;
  # If the current filename/path does exist, do nothing
  if ( -e $_)
  {
    $outputString = $outputString . $_ . "\n";
  }
  else
  {
    # Otherwise, let's check if it is in the search path
    my $filename = File::Spec->catfile( $LIBLIST_SEARCH_PATH, $_);
    if (-e $filename)
    {
      #print "Found " . $filename . "\n";
      $outputString = $outputString . $filename . "\n";
    }
    else
    {
      #print "Not Found " . $filename . "\n";
      $outputString = $outputString . $_ . "\n";
    }
  }
}


# Step 3
#   Output the resulting file, overwriting the input file
open(OUTFILE, ">", $LIBLIST_LINK_TXT_FILE) or die "Could not open file. $!";
print OUTFILE ($outputString);
close(OUTFILE);
