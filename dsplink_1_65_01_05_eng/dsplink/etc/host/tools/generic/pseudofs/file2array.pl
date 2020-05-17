#   ============================================================================
#   @file   file2array.pl
#
#   @path   $(DSPLINK)/etc/host/tools/generic/pseudofs/
#
#   @desc   This file converts a given file into character array by same name.
#           that can be compiled as a c program.
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


#   ****************************************************************************
#   * USAGE :                                                                  *
#   *                                                                          *
#   * file2array.pl inpfile outfile [verify]                                   *
#   ****************************************************************************

my ($debug) = 0 ;

#   ============================================================================
#   Define globals
#   ============================================================================

my ($sym_dirsep_msdos)  = '\\\\' ;       # Symbol for directory separator (MS DOS)
my ($sym_dirsep_unix)   = '/' ;          # Symbol for directory separator (UNIX)

my ($sym_equal)         = '=' ;          # Symbol used for assignment in input file

my ($tkn_filesys)       = 'FILESYS' ;    # Token for generated C file for filesystem
my ($tkn_addfile)       = 'ADDFILE' ;    # Token defining the files being included

my ($var_FileInfotable) = "FileDirectory" ;

my (@arr_TrueFilename)  = () ;           # Array of file names
my (@arr_ConvFilename)  = () ;           # Array of file names used for identifying the files
my (@arr_FileSize)      = () ;           # Array of file sizes
my (@arr_FileModifDate) = () ;           # Array of file modification date
my (@arr_FileContents)  = () ;           # Array of file contents

#   ============================================================================
#   Parse command line parameters
#   ============================================================================

if ($#ARGV < 0)
{
    die "USAGE : [PERL] file2array.pl inpfile [verify]\n" ;
}

my ($inpfile) = $ARGV [0] ;

my ($verify)  = 0 ;
if ($#ARGV == 1)
{
    if ($ARGV [2] eq "verify")
    {
        $verify  = 1 ;
    }
}

&Debug_1 ("Input  File : %s\n", $inpfile)  ;
&Debug_1 ("Verify Flag : %s\n", $verify)  ;


#   ============================================================================
#   Read the list of files to be converted
#   ============================================================================
&Debug_1 ("\nReading  input  file [%s]\n", $inpfile) ;

open (INPFILE, "$inpfile") || die "!! Couldn't open input file: $inpfile\n" ;

my ($incfile) ;
my ($outfile) ;
my (@files) ;
my ($file) ;

while (<INPFILE>)
{
    my ($dummy) ;

    chomp ;

    #   ------------------------------------------------------------------------
    #   Ignore comments
    #   ------------------------------------------------------------------------
    unless (/^#/)
    {
        #   --------------------------------------------------------------------
        #   Look for token specifying output file
        #   --------------------------------------------------------------------
        if (/^$tkn_filesys/)
        {
            ($dummy, $outfile) = split (/$sym_equal/) ;
            $outfile =~ s/^\s*(.*?)\s*$/$1/ ;   # Trim leading & trailing spaces
        }
        #   --------------------------------------------------------------------
        #   Look for token specifying file(s)
        #   --------------------------------------------------------------------
        if (/^$tkn_addfile/)
        {
            ($dummy, $incfile) = split (/$sym_equal/) ;
            $incfile =~ s/^\s*(.*?)\s*$/$1/ ;   # Trim leading & trailing spaces

            #   ----------------------------------------------------------------
            #   Add each file into an array for processing later
            #   ----------------------------------------------------------------
            @files = split (/\s+/, $incfile) ;  # Add files to the array

            foreach $file (@files)
            {
                push @arr_TrueFilename, $file ;
            }
        }
    }
}

close (INPFILE) ;

#   ============================================================================
#   Create output file and write content of files as arrays.
#   ============================================================================
&Debug_1 ("Creating output file [%s]\n\n", $outfile) ;

open (OUTFILE, ">$outfile") || die "!! Couldn't open output file: $outfile\n" ;

#   ----------------------------------------------------------------------------
#   Process each file specified and read necessary data into arrays
#   ----------------------------------------------------------------------------

$fileIdx = "0000" ;

foreach $file (@arr_TrueFilename)
{
    my ($name) ;
    my ($size) ;
    my ($modif) ;
    my (@contents) ;

    &Debug_1 ("\nReading file [%s]\n", $file) ;


    push (@arr_ConvFilename, $fileIdx) ;
    $fileIdx++ ;

    #   ------------------------------------------------------------------------
    #   Get file size and time of last modification
    #   ------------------------------------------------------------------------
    $size  = &GetFileSize    ($file) ;
    $modif = &GetFileModTime ($file) ;

    push (@arr_FileSize,      $size) ;
    push (@arr_FileModifDate, $modif) ;

    &Debug_1 ("  Size   : %d\n", $size) ;
    &Debug_1 ("  Modif  : %d\n", $modif) ;

    #   ------------------------------------------------------------------------
    #   Read contents of file into an array
    #   ------------------------------------------------------------------------
    @contents = ReadFileInArray ($file) ;

    push (@arr_FileContents,  [ @contents ]) ;
}

#   ----------------------------------------------------------------------------
#   Write to the output file as 'C' statements
#   ----------------------------------------------------------------------------
&WriteFileHeader    (OUTFILE) ;
&WriteFileNames     (OUTFILE) ;
&WriteFileContents  (OUTFILE) ;
&WriteFileDirectory (OUTFILE) ;

#   ----------------------------------------------------------------------------
#   Close output file
#   ----------------------------------------------------------------------------
&Debug_0 ("\nClose output file\n") ;

close (OUTFILE) ;

&Debug_1 ("\nDone !!!\n") ;

#   ============================================================================
#   Done !!!
#   ============================================================================


sub WriteFileHeader
{
    my ($handle) = shift @_ ;

    &Debug_0 ("Writing file header...\n") ;

    my ($header) = 'Caution ! This is a generated file.' ;

    $header = $header . ' It creates a pseudo-file system.' ;

    WriteComment ($handle, $header) ;

    print $handle "\n\n#include <gpptypes.h>\n\n" ;

    WriteComment ($handle, 'Structure defining the file details') ;

    print $handle "typedef struct FileDetails_tag {\n" ;
    print $handle "    CONST Char8 *   name    ;" ;
    print $handle "   /* File Name                 */\n" ;
    print $handle "    CONST Uint8 *   content ;" ;
    print $handle "   /* Pointer to file contents  */\n" ;
    print $handle "    Uint32          size    ;" ;
    print $handle "   /* File size                 */\n" ;
    print $handle "    Uint32          modTime ;" ;
    print $handle "   /* Time of last modification */\n" ;
    print $handle "} FileDetails ;\n\n" ;
}

#   ============================================================================
#   func    WriteFileNames
#
#   desc    Write the contents of input files as 'C' arrays to specified file.
#   ============================================================================
sub WriteFileNames
{
    my ($i) ;
    my ($handle)    = @_ ;

    &Debug_0 ("Writing file names...\n") ;

    WriteComment ($handle, "Name of file(s) included in the build") ;

    for ($i = 0 ; $i <= $#arr_TrueFilename ; $i++)
    {
        my ($fname)  = $arr_TrueFilename [$i] ;

        $fname =~ s/$sym_dirsep_msdos/$sym_dirsep_unix/g ;

        printf $handle "CONST Char8 name_%-6s = \"%s\" ;\n",
                       $arr_ConvFilename [$i] . ' []',
                       $fname ;
    }

    print $handle "\n" ;
}


#   ============================================================================
#   func    WriteFileContents
#
#   desc    Write the contents of input files as 'C' arrays to specified file.
#   ============================================================================
sub WriteFileContents
{
    my ($handle)    = @_ ;
    my ($filecount) = 0 ;
    my (@contents) ;
    my ($bytecount) ;

    &Debug_0 ("Writing file contents...\n") ;

    WriteComment ($handle, 'Content of file(s) included in the build') ;

    foreach $file (@arr_TrueFilename)
    {
        my ($verfile) = $file ;

        &Debug_1 ("  Converting file [%s]\n", $file) ;

        if ($verify == 1)
        {
            $verfile =~ s/\./\_ver./g ;

            &Debug_1 ("    => Verify : %s\n", $verfile) ;

            open (VERFILE, ">$verfile") || die "!! Couldn't open file : $verfile\n" ;
            binmode (VERFILE) ;
        }

        printf $handle "CONST Uint8 data_%s [] = {\n", $arr_ConvFilename [$filecount] ;

        @contents = @{$arr_FileContents [$filecount]} ;

        for ($bytecount = 0 ; $bytecount <= $#contents ; $bytecount++)
        {
            my ($wrapline) = $bytecount % 12 ;

            if ($wrapline == 0)
            {
                printf $handle "    " ;
            }

            printf $handle "0x%02X,", $contents [$bytecount] ;

            if ($wrapline == 11)
            {
                print $handle "\n" ;
            }
            else
            {
                print $handle " " ;
            }

            if ($verify == 1)
            {
                print VERFILE  chr ($contents [$bytecount]) ;
            }
        }

        printf $handle "0x%02X", $contents [$#contents] ;

        print $handle "\n} ;\n\n" ;

        if ($verify == 1)
        {
            print VERFILE  chr ($contents [$#contents]) ;
            close (VERFILE) ;

            &Debug_1 ("    => Length : %d\n", ($bytecount + 1)) ;
        }

        $filecount++ ;
    }
}


#   ============================================================================
#   func    WriteFileDirectory
#
#   desc    Write the contents of File Information Table to specified file.
#   ============================================================================
sub WriteFileDirectory
{
    my ($i) ;
    my ($handle) = shift @_ ;

    WriteComment ($handle, 'Define the pseudo-directory for file(s)') ;

    printf $handle "CONST FileDetails %s [] = {\n", $var_FileInfotable ;

    for ($i = 0 ; $i < $#arr_ConvFilename ; $i++)
    {
        printf $handle "    {name_%-6s data_%-6s %6d, %12d},\n",
                       $arr_ConvFilename [$i] . ',',
                       $arr_ConvFilename [$i] . ',',
                       $arr_FileSize [$i],
                       $arr_FileModifDate [$i] ;
    }

    printf $handle "    {name_%-6s data_%-6s %6d, %12d}\n",
                   $arr_ConvFilename [$#arr_ConvFilename] . ',',
                   $arr_ConvFilename [$#arr_ConvFilename] . ',',
                   $arr_FileSize [$#arr_FileSize],
                   $arr_FileModifDate [$#arr_FileModifDate] ;

    print $handle "} ;\n\n" ;

    WriteComment ($handle, 'Number of file(s) in the pseudo-directory') ;

    printf $handle "CONST Int32 FilesInDirectory = %d ;\n",
                   ($#arr_ConvFilename + 1) ;
}


#   ============================================================================
#   func    ReadFileInArray
#
#   desc    Read contents of specified file into an array.
#           The contents are read in binary mode.
#   ============================================================================
sub ReadFileInArray
{
    my ($file)   = shift (@_) ;
    my (@contents) ;

    open (INCFILE, "$file") || die "!! Couldn't open $file for input.\n" ;

    binmode (INCFILE) ;

    until (eof (INCFILE))
    {
        my ($char) = getc (INCFILE) ;

        push (@contents, ord ($char)) ;
    }

    close (INCFILE) ;

    return @contents ;
}

#   ============================================================================
#   func    WriteComment
#
#   desc    Write the "C" style comments with given text.
#   ============================================================================
sub WriteComment
{
    my ($handle) = shift @_ ;
    my ($text)   = @_ ;

    print $handle "/*  ------------------------------------" ;
    print $handle "----------------------------------------\n" ;
    print $handle " *  ", $text, "\n" ;
    print $handle " *  ------------------------------------" ;
    print $handle "----------------------------------------\n" ;
    print $handle " */\n" ;
}

#   ============================================================================
#   func    GetFileSize
#
#   desc    Get the size of specified file.
#   ============================================================================
sub GetFileSize
{
    my ($file) = @_ ;
    my (@info) ;
    my ($size) ;

    @info = stat ($file) ;

    $size = $info [7] ;

    return $size ;
}


#   ============================================================================
#   func    GetFileModTime
#
#   desc    Get the time of lst modification for specified file.
#   ============================================================================
sub GetFileModTime
{
    my ($file) = @_ ;
    my (@info) ;
    my ($mtime) ;

    @info = stat ($file) ;

    $mtime = $info [9] ;

    return $mtime ;
}


#   ============================================================================
#   func    Debug_0
#
#   desc    Print a string if debug level matches.
#   ============================================================================
sub Debug_0
{
    my ($string) = @_ ;

    if ($debug == 1)
    {
        print $string ;
    }
}

#   ============================================================================
#   func    Debug_1
#
#   desc    Print a format string and an argument if debug level matches.
#   ============================================================================
sub Debug_1
{
    my ($format)   = shift (@_) ;
    my ($argument) = @_ ;

    if ($debug == 1)
    {
        printf $format, $argument ;
    }
}
