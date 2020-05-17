#!/usr/bin/perl
#   ============================================================================
#   @file   staticloader.pl
#
#   @path   $(DSPLINK)/etc/host/scripts/msdos/loaderutils/static/
#
#   @desc   Generates source and command files corresponding to DSP executable
#           image specified. These generated files can be statically linked
#           into the ARM-side executable to get a pre-loaded DSP image.
#           The files generated are:
#           1. 'C' file containing an instance of the STATICLOADER_ImageInfo
#              structure required by the static loader.
#              This generated file can be built with the ARM-side example to
#              generate information used by the static loader.
#           2. 'H' file containing declaration of the instance of the
#              STATICLOADER_ImageInfo structure in the 'C' file. This file can
#              be included by the ARM-side application using the static loader.
#           3. 'C' file containing arrays of DSP sections. This file is
#              generated in the asp-psp source directory for Davinci, and
#              included in the build.
#           4. 'CMD' file containing linker placement of the DSP section arrays.
#              This file is generated in the asp-psp build directory for Davinci
#              and included in the linker command file for the GPP executable.
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

#-------------------------------------------------------------------------------
# Older revisions of Perl do not support "use warnings".  Can be replaced
# by invoking Perl with -w.
#-------------------------------------------------------------------------------
use warnings;
use strict;

#-------------------------------------------------------------------------------
# Include the XML parser package
#-------------------------------------------------------------------------------
use XML::Simple;


#-------------------------------------------------------------------------------
# Paths used
#-------------------------------------------------------------------------------
my $dsplinkDir      = $ENV {'DSPLINK'} ;
my $cgtoolsDir      = "c:\\ti-tools\\c6000\\cgtools" ;
my $aspPspSrc       = "L:\\asp-psp" ;

#-------------------------------------------------------------------------------
# Process the command line
#-------------------------------------------------------------------------------
if (@ARGV != 2) {
    print "  -------------------------------------------------------------------\n" ;
    print "  Usage: staticloader.pl <GPP o/p path> <DSP i/p file>\n" ;
    print "       <GPP o/p path>: Path where GPP-side files should be generated.\n" ;
    print "       <DSP i/p file>: Name (with path) of input DSP COFF executable.\n\n" ;
    print "  Example: staticloader.pl L:\\dsplink\\gpp\\src\\samples\\message\\PrOS L:\\dsplink\\dsp\\export\\BIN\\DspBios\\Davinci\\RELEASE\\message.out\n" ;
    print "  -------------------------------------------------------------------\n" ;
    exit (0) ;
}

my $gppOutFilePath  =  $ARGV [0] ;
my $filename        =  $ARGV [1] ;

print "  -------------------------------------------------------------------\n" ;
print "  CGTOOLS path used           : " . $cgtoolsDir . "\n" ;
print "  DSPLINK environment variable: " . $dsplinkDir . "\n" ;
print "  ASP-PSP path used           : " . $aspPspSrc  . "\n\n" ;
print "  Generating files ...\n\n" ;

#-------------------------------------------------------------------------------
# Get the file names.
#-------------------------------------------------------------------------------
my (@path) = split ('\\\\', $filename) ;
my $fnameext = pop @path ;
my ($fname, $fextn) = split ('\.', $fnameext) ;
my $dspOutFilePath = join ('\\', @path) ;

#-------------------------------------------------------------------------------
# Run the OFD utility to get the XML output
#-------------------------------------------------------------------------------
my $xml      = `$cgtoolsDir\\bin\\ofd6x -x $filename`;

#-------------------------------------------------------------------------------
# Parse the XML output
# XMLin API comes from XML::Simple
#-------------------------------------------------------------------------------
my $config   = XMLin($xml);

#-------------------------------------------------------------------------------
# Pointer to all 'section' entities
#-------------------------------------------------------------------------------
my $sections = $config->{'object_file'}->{'ti_coff'}->{'section'};
my $symbols  = $config->{'object_file'}->{'ti_coff'}->{'symbol_table'}->{'symbol'};
my($buff, $sectname);

#-------------------------------------------------------------------------------
# Declaration of the symbol for DSPLINK SHM base.
#-------------------------------------------------------------------------------
my $dsplinkShmBaseName  = "_DSPLINK_shmBaseAddress" ;
my $dsplinkShmBaseValue = 0 ;

#-------------------------------------------------------------------------------
# Open output files for writing
#-------------------------------------------------------------------------------
my $dsplinkGppCFile   = $gppOutFilePath . "\\" . $fname . "_staticloader.c" ;
my $dsplinkGppHFile   = $gppOutFilePath . "\\" . $fname . "_staticloader.h" ;
my $dsplinkDspCFile   = $aspPspSrc  . "\\system\\davinci\\pros\\davinci_evm\\dsplinkdsp.c" ;
my $dsplinkDspCmdFile = $aspPspSrc  . "\\system\\davinci\\pros\\davinci_evm\\build\\dsplinkdsp.cmd" ;

open (OUTGPPC,   ">$dsplinkGppCFile");
open (OUTGPPH,   ">$dsplinkGppHFile");
open (OUTDSPC,   ">$dsplinkDspCFile");
open (OUTCMD,    ">$dsplinkDspCmdFile");

#-------------------------------------------------------------------------------
# Get entry point.
#-------------------------------------------------------------------------------
my $entryPt = hex($config->{'object_file'}->{'ti_coff'}->{'optional_file_header'}->{'entry_point'});
my $argsAddr = 0 ;
my $argsSize = 0 ;

#-------------------------------------------------------------------------------
# Open input COFF file (.out) for reading
#-------------------------------------------------------------------------------
open (OBJFILE, "<$filename") || die "Could not open input file.\n";
binmode(OBJFILE);

#-------------------------------------------------------------------------------
# Write the MEMORY directive in linker command file.
#-------------------------------------------------------------------------------
print OUTCMD "MEMORY\n{\n";
print OUTCMD "    DSPLINK_DDR:          org= 0x8FF00000 len= 0x100000\n";
print OUTCMD "    DSPLINK_DSPIRAM:      org= 0x11800000 len= 0x10000\n";
print OUTCMD "    DSPLINK_DSPL1DRAM:    org= 0x11F04000 len= 0xC000\n";
print OUTCMD "}\n\n";

print OUTCMD "SECTIONS\n{\n";

#-------------------------------------------------------------------------------
# Get the symbol value for the DSPLINK SHM base.
#-------------------------------------------------------------------------------
$dsplinkShmBaseValue = get_symbol_value($dsplinkShmBaseName);

#-------------------------------------------------------------------------------
# For each 'section' entity
#-------------------------------------------------------------------------------
foreach $sectname (keys %$sections)
{
    #---------------------------------------------------------------------------
    # Find section's name, length and address
    #---------------------------------------------------------------------------
    my $sect  = $sections->{$sectname};
    my $size  = hex($sect->{'raw_data_size'});
    my $paddr = hex($sect->{'physical_addr'});

    #---------------------------------------------------------------------------
    # Find pointer to section's raw data inside the COFF file
    #---------------------------------------------------------------------------
    my $ptr   = hex($sect->{'file_offsets'}->{'raw_data_ptr'});

    #---------------------------------------------------------------------------
    # if section is non-zero length and there is data in the file ...
    #---------------------------------------------------------------------------
    if ($size != 0 && $ptr != 0    &&
    #---------------------------------------------------------------------------
    # and it is an initialized section ...
    #---------------------------------------------------------------------------
       (not defined $sect->{'bss'})   &&
    #---------------------------------------------------------------------------
    # and it is not a copy section ...
    # (copy sections are information of some kind that is not to be
    # loaded to the target.  Best example is .cinit when linked under
    # -cr.  Dwarf debug sections are also copy sections.)
    #---------------------------------------------------------------------------
      (not defined $sect->{'copy'})  &&
    #---------------------------------------------------------------------------
    # and not a dummy section ...
    # (dummy is a legacy section type typically not used)
    #---------------------------------------------------------------------------
      (not defined $sect->{'dummy'}) &&
    #---------------------------------------------------------------------------
    # and not a noload section.
    # (noload sections come up when you are doing particularly complicated
    # partial linking tricks.  As the name implies, the section is not to
    # loaded.)
    #---------------------------------------------------------------------------
      (not defined $sect->{'noload'}))
    {
        #-----------------------------------------------------------------------
        # only initialized sections will pass through here.
        # copy it's raw data from the COFF file into the .c file
        # and copy section's name and size into the .h file
        #-----------------------------------------------------------------------
        seek(OBJFILE, $ptr, 0) || die "seek:$!";
        read(OBJFILE, $buff, $size);
        print_c_struct($sectname, $size, $paddr,  $buff);

        #-----------------------------------------------------------------------
        # Store the address and size of .args section.
        #-----------------------------------------------------------------------
        if ("$sectname" eq ".args")
        {
            $argsAddr = $paddr;
            $argsSize = $size;
        }
    }
}

print OUTCMD "}\n";


#-------------------------------------------------------------------------------
# Write GPP 'C' file with information required for static loader
#-------------------------------------------------------------------------------
print OUTGPPC "/** ============================================================================\n";
print OUTGPPC " *  \@name   " . uc($fname) . "_DspStaticLoaderInfo\n";
print OUTGPPC " *\n";
print OUTGPPC " *  \@desc   Array containing information of the DSP executable within the\n";
print OUTGPPC " *          generated source file as required by the Static Loader.\n";
print OUTGPPC " *          The variable below corresponds to type STATICLOADER_ImageInfo.\n";
print OUTGPPC " *  ============================================================================\n";
print OUTGPPC " */\n";
print OUTGPPC "const unsigned long " . uc($fname) . "_DspStaticLoaderInfo [4] = {\n";
printf OUTGPPC "    0x%x,\n", $entryPt;
printf OUTGPPC "    0x%x,\n", $argsAddr;
printf OUTGPPC "    0x%x,\n", $argsSize;
printf OUTGPPC "    0x%x\n",  $dsplinkShmBaseValue;
print OUTGPPC "} ;\n";
close OUTGPPC;

#-------------------------------------------------------------------------------
# Write GPP 'H' file with information required for binary loader
#-------------------------------------------------------------------------------
print OUTGPPH "/** ============================================================================\n";
print OUTGPPH " *  \@name   " . uc($fname) . "_DspStaticLoaderInfo\n";
print OUTGPPH " *\n";
print OUTGPPH " *  \@desc   Array containing information of the DSP executable within the\n";
print OUTGPPH " *          generated source file as required by the Static Loader.\n";
print OUTGPPH " *  ============================================================================\n";
print OUTGPPH " */\n";
print OUTGPPH "extern const unsigned long " . uc($fname) . "_DspStaticLoaderInfo [4] ;\n";
close OUTGPPH;


#-------------------------------------------------------------------------------
# Print where the files are generated
#-------------------------------------------------------------------------------
print "  Generated files:\n" ;
print "  1. " . $dsplinkGppCFile   . "\n" ;
print "  2. " . $dsplinkGppHFile   . "\n" ;
print "  3. " . $dsplinkDspCFile   . "\n" ;
print "  4. " . $dsplinkDspCmdFile . "\n" ;
print "  -------------------------------------------------------------------\n" ;


#-------------------------------------------------------------------------------
# Close files and exit
#-------------------------------------------------------------------------------
close OBJFILE;
close OUTDSPC;
close OUTCMD;
exit(0);

################################################################################
# PRINT_C_STRUCT - Output contents of one section
################################################################################
sub print_c_struct
{
    my($sectname, $size, $paddr, $buff) = @_;
    my($i);

    #---------------------------------------------------------------------------
    # Change any non-alphanumeric characters in section name to '_'
    #---------------------------------------------------------------------------
    $sectname =~ s/[^A-Za-z0-9]/_/g;

    #---------------------------------------------------------------------------
    # Output section header to C file
    #---------------------------------------------------------------------------
    printf OUTDSPC "#pragma DATA_SECTION (%s, \".DSPLINK%s\");\n", $sectname, $sectname;
    print  OUTDSPC "/*********************************************************\n";
    printf OUTDSPC "** %s[0x%x]: paddr = 0x%08x \n",
                          $sectname, $size, $paddr;
    print  OUTDSPC "**********************************************************/\n";
    printf OUTDSPC "const unsigned char %s[0x%x] = {", $sectname, $size;
    printf OUTCMD "    .DSPLINK%s > 0x%x\n", $sectname, $paddr;

    for ($i=0; $i<$size; $i++)
    {
        #-----------------------------------------------------------------------
        # Start a new line every 13 bytes.  Comes to about 80 chars per line.
        #-----------------------------------------------------------------------
        print OUTDSPC "\n" if (($i % 13) == 0);
        print OUTDSPC "0x". sprintf("%02x", ord(substr($buff, $i, 1))) . ", ";
    }

    print OUTDSPC "} ;\n\n";
}

################################################################################
# GET_SYMBOL_VALUE - Get value of a symbol.
################################################################################
sub get_symbol_value
{
    my($getsymname) = @_;
    my $symname;
    my $getvalue = 0;

    #---------------------------------------------------------------------------
    # For each 'symbol' entity
    #---------------------------------------------------------------------------
    foreach $symname (keys %$symbols)
    {
        #-----------------------------------------------------------------------
        # Check if the symbol name matches the required name.
        #-----------------------------------------------------------------------
        if ($symname eq $getsymname)
        {
            #-------------------------------------------------------------------
            # Find section's name, length and address
            #-------------------------------------------------------------------
            my $sym   = $symbols->{$symname};
            $getvalue = hex($sym->{'value'});
        }
    }

    return $getvalue ;
}
