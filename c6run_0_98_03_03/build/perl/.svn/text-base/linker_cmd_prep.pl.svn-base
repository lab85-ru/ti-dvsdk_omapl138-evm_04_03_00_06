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
use Text::Balanced qw(extract_bracketed);

# Select Debug output
my $debug = 0;

my $INPUT_LINK_CMD_FILE   = shift or die "No input linker command file given.";
my $FINAL_LINK_CMD_FILE   = $INPUT_LINK_CMD_FILE;
my $LIBLIST_LINK_TXT_FILE = $INPUT_LINK_CMD_FILE;
my $OBJLIST_LINK_TXT_FILE = $INPUT_LINK_CMD_FILE;
my $partial_string;
my $final_string;
my $string;

# Update the output linker command file names
$FINAL_LINK_CMD_FILE=~s/^(.*)\.(\w+)/$1_final\.$2/g;
$LIBLIST_LINK_TXT_FILE=~s/^(.*)\.\w+/$1_liblist\.txt/g;
$OBJLIST_LINK_TXT_FILE=~s/^(.*)\.\w+/$1_objlist\.txt/g;

# Based on description of linker command files:
#   Linker command files are ASCII files that contain one or more of the following:
#   • Input filenames, which specify object files, archive libraries, or other command files. (If a command file
#     calls another command file as input, this statement must be the last statement in the calling command
#     file. The linker does not return from called command files.)
#   • Linker options, which can be used in the command file in the same manner that they are used on the
#     command line
#   • The MEMORY and SECTIONS linker directives. The MEMORY directive defines the target memory
#     configuration (see Section 7.5.3). The SECTIONS directive controls how sections are built and
#     allocated (see Section 7.5.4.)
#   • Assignment statements, which define and assign values to global symbols

# Step 0 - Make sure input file exists
if (! -e $INPUT_LINK_CMD_FILE)
{
  die "File $INPUT_LINK_CMD_FILE does not exit!";
}

# Step 1
#   Go to the last non-empty line of the linker command file and if it is a non-binary file filename
#   import its contents to the current file. Save, then repeat.
{
  my $lineNum = 0;
  my $importLineNum = -1;
  my $importString = "";

  do
  {
    $lineNum = 0;
    $importLineNum = -1;
    $importString = "";
  
    open(INFILE, "<", $INPUT_LINK_CMD_FILE) or die "Could not open file. $!";
  
    my @cmd_data = <INFILE>;
    
    # Scroll backward through file to last line with non-whitespace
    # It is assumed that this line could be one calling another linker command
    # file (assuming a well-formatted linker command file).
    do
    {
      $lineNum--;
      $_ = @cmd_data[$lineNum];
    }
    until ( /^\s*\S+/ );
    
    # Get last element of line, strip any quotes
    chomp;  
    my @files = split();
    my $fileName = $files[$#files];
    $fileName =~ s/^"(.*)"$/$1/;
    
    if ($debug)
    {
      print "Detected potential linker command file filename = $fileName\n";
    }
    
    # Check if it is a file that exists
    if (-e $fileName)
    {
      # Check to see if it is a text file
      if (-T $fileName)
      {
        if ($debug)
        {
          print "Linker command file found for import: $fileName\n";
        }
        local $/=undef;
        open FILE, $fileName or die "Couldn't open file: $!";
        $importString = <FILE>;
        $importLineNum = $lineNum + scalar(@cmd_data);
        close FILE;
      }
    }
    
    close(INFILE);
    
    # Only update the linker file if an import linker command file was located
    if ($importLineNum != -1)
    {
      open(INFILE, "<", $INPUT_LINK_CMD_FILE) or die "Could not open file. $!";
      $final_string = "";
      $lineNum = 0;
      while (<INFILE>)
      {
        if ($importLineNum == $lineNum)
        {
          $final_string = $final_string . $importString;
        }
        else
        {
          $final_string = $final_string . $_;
        }
        $lineNum++;
      }
      close(INFILE);
    
      open(OUTFILE, ">", $INPUT_LINK_CMD_FILE) or die "Could not open file. $!";
      print OUTFILE ($final_string);
      close(OUTFILE);
    }
  }
  until ( $importLineNum == -1);
  
  if ($debug)
  {
    print "Linker command file import done!\n";
  }
}

# Step 2
#   Collect the MEMORY and SECTIONS directives
my $finalMemoryText = "";
my $finalSectionsText = "";
my $leftoverText;
{
  my $extracted;
  my $skipped;
  my $remainder;

  local $/ = undef;

  # Get the MEMORY directives using Text::Balanced::extract_bracketed
  open FILE, $INPUT_LINK_CMD_FILE or die "Couldn't open file: $!";  
  $remainder = <FILE>;
  close(FILE);
  $leftoverText = "";
  do
  {
    ($extracted, $remainder, $skipped) = extract_bracketed($remainder, '{}', qr/.*?MEMORY.*?(?={)/s);
    if ($extracted ne "")
    {
      $finalMemoryText = $finalMemoryText . "MEMORY\n" . $extracted . "\n\n";
      $leftoverText = $leftoverText . $skipped;
    }
    else
    {
      $leftoverText = $leftoverText . $remainder;
    }
  }
  until ($extracted eq ""); 


  # Get the SECTIONS directives using Text::Balanced::extract_bracketed
  $remainder = $leftoverText;
  $leftoverText = "";
  do 
  {
    ($extracted, $remainder, $skipped) = extract_bracketed($remainder, '{}', qr/.*?SECTIONS.*?(?={)/s);
    if ($extracted ne "")
    {
      $finalSectionsText = $finalSectionsText . "SECTIONS\n" . $extracted . "\n\n";
      $leftoverText = $leftoverText . $skipped;
    }
    else
    {
      $leftoverText = $leftoverText . $remainder;
    }
  }
  until ($extracted eq "");  
  
  # Remove leftover SECTIONS and MEMORY prefixes in the original text
  $leftoverText =~ s/MEMORY//gsm;
  $leftoverText =~ s/SECTIONS//gsm;
}

# Step 3
#   Collect all linker options (assume one per line)
#   Library linker options are only in partial file
#   Strip out any RTS lib references
my $finalOptionsText = "";
my $libListText = "";
while ($leftoverText =~ /^\s*(-{1,2}.+)$/mg)
{
  my $optionText = $1;
  
  if ($optionText =~ /-l.+/)
  {
    if ($optionText !~ /-lrts\w+\.lib/)
    {
      $optionText =~ s/-l(.+)/$1/;
      $optionText =~ tr/"//d;
      $optionText =~ s#/\*[^*]*\*+([^/*][^*]*\*+)*/|("(\\.|[^"\\])*"|'(\\.|[^'\\])*'|.[^/"'\\]*)#defined $2 ? $2 : ""#gse;
      $libListText = $libListText . $optionText . "\n";
    }
  }
  elsif ($optionText =~ /--library=.+/)
  {
    if ($optionText !~ /--library=rts\w+\.lib/)
    {
      $optionText =~ s/--library=(.+)/$1/;
      $optionText =~ tr/"//d;      
      $optionText =~ s#/\*[^*]*\*+([^/*][^*]*\*+)*/|("(\\.|[^"\\])*"|'(\\.|[^'\\])*'|.[^/"'\\]*)#defined $2 ? $2 : ""#gse;
      $libListText = $libListText . $optionText
    }
  }
  elsif ($optionText =~ /--dynamic/)
  {
    # This option messes up the partial link
    $finalOptionsText   = $finalOptionsText . $optionText . "\n";
  }
  elsif ($optionText =~ /-e.*/)
  {
    $finalOptionsText   = $finalOptionsText . $optionText . "\n";
  }
  else
  {
    $finalOptionsText   = $finalOptionsText . $optionText . "\n";
  }
}
$leftoverText =~ s/^\s*(-{1,2}.+)$//mg;


# Step 4
#   Collect all symbol assignment operators.
my $finalAssignmentText = "";
while ($leftoverText =~ /(\w+\s*[\+\-\*\/]*=\s*.+)\s*\;/g)
{
  $finalAssignmentText = $finalAssignmentText . $1 . ";\n";
}
$leftoverText =~ s/(\w+\s*[\+\-\*\/]*=\s*.+)\s*\;//g;


# Step 5
#   Strip out all C-style comments
#   Strip out all empty lines
$leftoverText =~ s#/\*[^*]*\*+([^/*][^*]*\*+)*/|("(\\.|[^"\\])*"|'(\\.|[^'\\])*'|.[^/"'\\]*)#defined $2 ? $2 : ""#gse;
$leftoverText =~ s/^\s+$//g;
$leftoverText =~ s/\n{2,}/\n/g;


# Step 6
#   Get object files list for partial linker command file
my $objListText = "";
while ($leftoverText =~ /^(.+)$/g)
{
  $_ = $1;
  chomp;
  my @files = split();
  
  for (my $i=0; $i<=$#files; $i++)
  {
    my $fileName = $files[$i];
    $fileName =~ s/^"([^"]+)"$/$1/;
          
    if ((-e $fileName) && (-B $fileName))
    {
      $objListText = $objListText . "\"$fileName\"\n";
    }
  }
}
$leftoverText =~ s/^(.+)$//g;

# $leftoverText should now be empty
if ($debug)
{
  print "Object File List: \n";
  print $objListText;
  
  print "Final MEMORY directives: \n";
  print $finalMemoryText;
  
  print "Final SECTIONS directives: \n";
  print $finalSectionsText;

  print "Final Options List: \n";
  print $finalOptionsText;

  print "Final Assignment List: \n";
  print $finalAssignmentText;

  print "Left over stuff: \n";
  print $leftoverText;
}

# Create the output partial and final linker command files
#print OUTFILE ($objListText);

  
open(OUTFILE, ">", $FINAL_LINK_CMD_FILE) or die "Could not open file. $!";
print OUTFILE ($finalOptionsText);
print OUTFILE ($finalAssignmentText);
print OUTFILE ($finalMemoryText);
print OUTFILE ($finalSectionsText);
close(OUTFILE);

open(OUTFILE, ">", $LIBLIST_LINK_TXT_FILE) or die "Could not open file. $!";
print OUTFILE ($libListText);
close(OUTFILE);

open(OUTFILE, ">", $OBJLIST_LINK_TXT_FILE) or die "Could not open file. $!";
print OUTFILE ($objListText);
close(OUTFILE);
