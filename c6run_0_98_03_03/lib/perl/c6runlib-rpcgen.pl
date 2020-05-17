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

use lib "$ENV{C6RUN_INSTALL_DIR}/lib/perl";

use strict;
use GCC::TranslationUnit;
use Text::Balanced qw(extract_bracketed);

# Class for storing data about a type
package type;

#Constructor and initialization
sub new
{
  my $class = shift;
  my $self = { @_ };
  bless $self, $class;
  $self->_init();
  return $self;
}
sub _init {
  my $self = shift;
  if (not defined $self->{isPointer})
  {
    $self->{isPointer} = 0;
  }
  if (not defined $self->{isCompound})
  {
    $self->{isCompound} = 0;
  }
}

# Object Accessor methods
sub string
{
  my $self = shift;
  return $self->_get_type_string();
}

sub size
{
  my $self = shift;
  my $data = shift;
  $self->{size} = $data if defined $data;  
  return $self->{size};
}

sub qual
{
  my $self = shift;
  my $type = $self->{tuNode};
  
  return $type->qual;
}

sub isPointer
{
  my $self = shift;
  my $type = $self->{tuNode};
  
  # Process pointer argument types
  if($type->isa('GCC::Node::pointer_type'))
  {
    return 1;
  }
  return 0;
}

sub typePointedTo
{
  my $self = shift;
  return $self->{typePointedTo};
}

sub isCompound
{
  my $self = shift;
  my $type = $self->{tuNode};
  
  if($type->isa('GCC::Node::record_type'))
  {
    return 1;
  }
  return 0;
}

sub tuNode
{
  my $self = shift;
  my $data = shift;
  $self->{tuNode} = $data if defined $data;  
  return $self->{tuNode};
}

sub _get_type_string
{
  my $self = shift;
  my $type = $self->{tuNode};
  my $typeString;
  
  # Process pointer argument types
  if($type->isa('GCC::Node::pointer_type'))
  {
    if ($type->type->name->isa('GCC::Node::identifier_node'))
    {
      my $ptrDataType = $type->type->name->identifier;
    }
    else
    {
      my $ptrDataType = $type->type->name->name->identifier;
    }
    my $pointedToType = new type(
      tuNode => $type->ptd
    );
    $self->{typePointedTo} = $pointedToType;
    $typeString = $pointedToType->string . " " . $type->thingy . $type->qual;
  }
      
  # Process enumerated argument types
  elsif($type->isa('GCC::Node::enumeral_type'))
  {
    if ($type->name->isa('GCC::Node::identifier_node'))
    {
      my $enumType = $type->name->identifier;
      $typeString = $type->qual . " enum $enumType";
    }
    elsif($type->name->isa('GCC::Node::type_decl'))
    {
      $typeString = $type->qual . " " . $type->name->name->identifier;
    }
  }
  
  # Process union and struct types
  elsif($type->isa('GCC::Node::record_type'))
  {
    if($type->isa('GCC::Node::union_type'))
    {
      if ($type->name->isa('GCC::Node::identifier_node'))
      {
        my $unionType = $type->name->identifier;
        $typeString = $type->qual . " union $unionType";
      }
      elsif($type->name->isa('GCC::Node::type_decl'))
      {
        $typeString = $type->qual . " " . $type->name->name->identifier;
      }
    }
    else
    {
      if ($type->name->isa('GCC::Node::identifier_node'))
      {
        my $structType = $type->name->identifier;
        $typeString = $type->qual . " struct $structType";
      }
      elsif($type->name->isa('GCC::Node::type_decl'))
      {
        $typeString = $type->qual . " " . $type->name->name->identifier;
      }
    }
  }

  # Process integer argument types
  elsif($type->isa('GCC::Node::integer_type'))
  {
    $typeString = $type->qual . " " . $type->name->name->identifier;
  }

  # Process real (float and double) argument types
  elsif($type->isa('GCC::Node::real_type'))
  {
    $typeString = $type->qual . " " . $type->name->name->identifier;          
  }
  
  elsif($type->isa('GCC::Node::void_type'))
  {
    $typeString = $type->qual . " " . $type->name->name->identifier;
  }
  
  elsif($type->isa('GCC::Node::boolean_type'))
  {
    $typeString = $type->qual . " " . $type->name->name->identifier;
  }

  # Report error for any argument type not processed above
  else
  {
    die "Error: Unsupported argument type", ref($type);
  }

  $typeString =~ s/^\s+//; #remove leading spaces
  $typeString =~ s/\s+$//; #remove trailing spaces
  
  return $typeString;
}


# Class for storing data about a function argument
package argument;

#Constructor and initialization
sub new
{
  my $class = shift;
  my $self = { @_ };
  bless $self, $class;
  $self->_init();
  return $self;
}
sub _init {
  my $self = shift;
  if (not defined $self->{pointerType})
  {
    $self->{pointerType} = "INOUTBUF";
  }
}

# Object Accessor methods
sub name
{
  my $self = shift;
  my $data = shift;
  $data =~ s/^\s+//; #remove leading spaces
  $data =~ s/\s+$//; #remove trailing spaces
  $self->{name} = $data if defined $data;  
  return $self->{name};
}
sub argType
{
  my $self = shift;
  my $data = shift;
  $self->{argType} = $data if defined $data;  
  return $self->{argType};
}
sub argTypeSourceString
{
  my $self = shift;
  my $data = shift;
  $data =~ s/^\s+//; #remove leading spaces
  $data =~ s/\s+$//; #remove trailing spaces
  $self->{argTypeSourceString} = $data if defined $data;  
  return $self->{argTypeSourceString};
}
sub argSize
{
  my $self = shift;
  return $self->argType->size;
}
sub isPointer
{
  my $self = shift;
  return $self->argType->isPointer;
}
sub isCompound
{
  my $self = shift;
  return $self->argType->isCompound;
}
sub pointerType
{
  my $self = shift;
  my $data = shift;
  $self->{pointerType} = $data if defined $data;  
  return $self->{pointerType};
}
sub tuNode
{
  my $self = shift;
  my $data = shift;
  $self->{tuNode} = $data if defined $data;  
  return $self->{tuNode};
}

# Object utility methods
sub print
{
  my $self = shift or die "No argument type given";
  my $indentVal = shift;
  my $indent = "";
  if (defined($indentVal))
  {
    while ($indentVal > 0)
    {
      $indent = $indent . " ";
      $indentVal--;
    }
  }
  
  print $indent,"name:                ",$self->name,"\n";
  print $indent,"argType:             ",$self->argType->string,"\n";
  print $indent,"argTypeSourceString: ",$self->argTypeSourceString,"\n";
  print $indent,"isPointer:           ",$self->isPointer,"\n";
  if ($self->isPointer == 1)
  {
    print $indent,"pointerType:         ",$self->pointerType,"\n";
  }
  print $indent,"isCompound:          ",$self->isCompound,"\n";
  print "\n";
}



# Class for storing data about a function
package function;

my @Functions;
my $InputSourceFile;
my $InputSourceFileShort;
my $PreprocessedSourceFile;
my $PreprocessedSourceFileShort;

#Constructor and initialization
sub new
{
  my $class = shift;
  my $self = { @_ };
  bless $self, $class;
  $self->_init();
  return $self;
}
sub _init {
  my $self = shift;
  if (not defined $self->{isVariadic})
  {
    $self->{isVariadic} = 0;
  }  
  push @Functions, $self;
}

# Class accessor methods
sub functionCount                 { return scalar(@Functions);            }
sub functions                     { return @Functions;                    }
sub inputSourceFile               { return $InputSourceFile;              }
sub inputSourceFileShort          { return $InputSourceFileShort;         }
sub preprocessedSourceFile        { return $PreprocessedSourceFile;       }
sub preprocessedSourceFileShort   { return $PreprocessedSourceFileShort;  }

# Class utility methods
sub setSourceFileNames
{
  my $class       = shift;
  my $data        = shift or die "setSourceFileNames(): No filename provided";
  my $outputPath  = shift or die "setSourceFileNames(): No output path provided";
  
  $InputSourceFile = $data;
  
  # Get short form (no paths, etc.) of the source file name 
  $InputSourceFileShort = $InputSourceFile;
  $InputSourceFileShort=~s/^(.*\/)*(.*)\.(\w+)/$2\.$3/g;
  
  # Create the preprocessed source file name
  $PreprocessedSourceFile = $InputSourceFile;
  $PreprocessedSourceFile =~ s/^(.*\/)*(.*)\.(\w+)/$outputPath$2\.pp\.$3/g;
  
  # Create the short preprocessed source file name
  $PreprocessedSourceFileShort = $PreprocessedSourceFile;
  $PreprocessedSourceFileShort =~ s/^(.*\/)*(.*)\.(\w+)/$2\.$3/g;
}

sub findFunction
{
  my $class = shift;
  my $fxnName = shift;
  foreach my $fxn(@Functions)
  {
    if ($fxnName eq $fxn->name())
    {
      return $fxn;
    }
  }
  return undef;
}



# Object accessor methods
sub name
{
  my $self = shift;
  my $data = shift;
  $data =~ s/^\s+//; #remove leading spaces
  $data =~ s/\s+$//; #remove trailing spaces
  $self->{name} = $data if defined $data;  
  return $self->{name};
}
sub origSourceLine
{
  my $self = shift;
  my $data = shift;
  $self->{origSourceLine} = $data if defined $data;  
  return $self->{origSourceLine};
}
sub procSourceLine
{
  my $self = shift;
  my $data = shift;
  $self->{procSourceLine} = $data if defined $data;  
  return $self->{procSourceLine};
}
sub returnType
{
  my $self = shift;
  my $data = shift;
  $self->{returnType} = $data if defined $data;  
  return $self->{returnType};
}
sub argCnt
{
  my $self = shift;
  my $data = shift;
  $self->{argCnt} = $data if defined $data;  
  return $self->{argCnt};
}
sub args
{
  my $self = shift;
  my $data = shift;
  $self->{args} = $data if defined $data;  
  return $self->{args};
}
sub isVariadic
{
  my $self = shift;
  my $data = shift;
  $self->{isVariadic} = $data if defined $data;  
  return $self->{isVariadic};
}
sub tuNode
{
  my $self = shift;
  my $data = shift;
  $self->{tuNode} = $data if defined $data;  
  return $self->{tuNode};
}

# Object utility methods
sub print
{
  my $self = shift;
  print "name:                  ",$self->name,"\n";
  print "origSourceLine:        ",$self->origSourceLine,"\n";
  print "procSourceLine:        ",$self->procSourceLine,"\n";
  print "returnType:            ",$self->returnType->string,"\n";
  print "argCnt:                ",$self->argCnt,"\n";
  print "args:\n";
  for (my $i = 0; $i < $self->argCnt; $i++)
  {
    $self->args->[$i]->print(2);
  }
  print "\n";
}

package main;

# Declare local functions
sub get_type($)
{
  my $type = shift;
  my $myType = new type(
    tuNode      => $type
  );
  
  return $myType;
}

sub get_argument($)
{
  my $arg = shift;
  my $myArgument = new argument(
    name                => $arg->name->identifier,
    argType             => get_type($arg->type),
    tuNode              => $arg
  );
  
  return $myArgument;
}

sub get_fxn_paramlist_from_source($$)
{
  my $fxn = shift or die "No function reference provided";
  my $source = shift or die "No source type provided";
  
  my $cFile;
  my $sourceLine;
  
  # Validate source options
  $source =~ tr/a-z/A-Z/;
  if (($source ne "ORIGINAL") && ($source ne "PROCESSED"))
  {
    die("Invalid source type parameter");
  }  

  # Select appropriate source filename and source line number
  if ($source eq "ORIGINAL")
  {
    $cFile = function->inputSourceFile;
    $sourceLine = $fxn->origSourceLine;  
  }
  else
  {
    $cFile = function->preprocessedSourceFile;
    $sourceLine = $fxn->procSourceLine;
  }
  my $fxnName = $fxn->name;


  my $ind = 0;
    
  # String for returned list of parameters
  my $functionText = "";
  my $param_list = "";
      
  if (1)
  {
    open(C_SRC_FILE, '<',  $cFile) or die $!;
    
    # Read the whole file into an array (this may be a problem for
    # very large C files, but that's bad coding practice anyways)
    my @c_data = <C_SRC_FILE>;
    
    # Line count and index variables
    my $lineCount = 1;
    my $beforeLineCount = 1;
    my $afterLineCount = -1;
    
    # Find the opening brace in the source, which starts the function body
    while ($afterLineCount <= (scalar(@c_data) - $sourceLine))
    {
      $_ = @c_data[$sourceLine+$afterLineCount];
      if (/\{/)
      {
        # print "\nFound opening brace on line ", $sourceLine+$afterLineCount,": ", $_,"\n";
        last;
      }
      $afterLineCount++;
    }
    
    # Now go backwards and find the closing parentheses of the parameter list
    while ($beforeLineCount <= $sourceLine)
    {
      $_ = @c_data[$sourceLine-$beforeLineCount];
      if (/\)/)
      {
        # print "\nFound closing parentheses on line ", $sourceLine-$beforeLineCount,": ", $_,"\n";
        last;
      }
      $beforeLineCount++;
    }
    
    # Now go backwards some more and find the opening parentheses of the parameter list
    while ($beforeLineCount <= $sourceLine)
    {
      $_ = @c_data[$sourceLine-$beforeLineCount];
      if (/\(/)
      {
        # print "\nFound opening parentheses on line ", $sourceLine-$beforeLineCount,": ", $_,"\n";
        last;
      }
      $beforeLineCount++;
    }
    
    # Now find the functionName in the source
    while ($beforeLineCount <= $sourceLine)
    {
      $_ = @c_data[$sourceLine-$beforeLineCount];
      if (/\s+$fxnName/)
      {
        # print "\nFound function name on line ", $sourceLine-$beforeLineCount,": ", $_,"\n";
        last;
      }
      $beforeLineCount++;
    }  

    # Combine all source lines from functionName line to sourceLine
    $lineCount = $beforeLineCount + $afterLineCount;
    do
    {
      $_ = @c_data[$sourceLine-$beforeLineCount];
      chomp;
      s/^\s+//; #remove leading spaces
      s/\s+$//; #remove trailing spaces
      
      $functionText = $functionText . $_ . "\n";
      $beforeLineCount--;
      $lineCount--;
    }
    until ( $lineCount == 0 );
    
    $ind = index($functionText,$fxnName);
    $functionText = substr($functionText,$ind,9999);
  }
  else
  {
    # FIXME - maybe a better usage (for pre-processed source only
    open(C_SRC_FILE, '<',  $cFile) or die $!;   
    my $c_data;
    {
      local $/ = undef;
      $c_data = <C_SRC_FILE>;
    }
    close(C_SRC_FILE);
  
    # Get parameter list
    my $before;
    my $after;
    my $found;
    ($before, $found, $after) = extract_bracketed($c_data, '()', qr/.*?$fxnName.*?(?=\()/s); 
    
    print $before,"\n";
    print $found,"\n";
    print $after,"\n";
    $functionText = $found;
    
    print $functionText;
  }

  # Get parameter list from function definition text
  $ind = 0;
  $ind = index($functionText,'(',$ind);
  if ($ind != -1)
  {
    my $startIndex = $ind + 1;
    my $stopIndex;
    # Find first closing parenthesis after first opening one
    $ind = index($functionText,')',$startIndex);
    if ($ind != -1)
    {
      $stopIndex = $ind - 1;
    }
    else
    {
      die "Function definition, $functionText, does not have valid parameter list";
    }
    $param_list = substr($functionText,$startIndex,$stopIndex - $startIndex + 1);
  }
  else
  {
    die "Function definition, $functionText, does not have valid parameter list";
  }
  
  $param_list =~ s/^\s+//; #remove leading spaces
  $param_list =~ s/\s+$//; #remove trailing spaces
  
  # Get list of params and clean them up
  my @params = split /,/, $param_list;
  for (my $i=0; $i <= $#params; $i++)
  {
    $params[$i] =~ s/^\s+//; #remove leading spaces
    $params[$i] =~ s/\s+$//; #remove trailing spaces
  }

  # Clean up the file we opened
  close(C_SRC_FILE);
  
  # Return the cleaned-up function parameter list
  return (\@params);
}

sub update_fxn_args($)
{
  my $fxn  = shift or die "TU node not provided";
  my $node = $fxn->tuNode;
  
  my @args;
  my @argListFromOrigSrc;
  my @argListFromProcSrc;
 
  # Get the function argument list from the original and processed source files 
  my $argListFromSrcRef = get_fxn_paramlist_from_source($fxn,"ORIGINAL");
  @argListFromOrigSrc = @{$argListFromSrcRef};
  $argListFromSrcRef = get_fxn_paramlist_from_source($fxn,"PROCESSED");
  @argListFromProcSrc = @{$argListFromSrcRef};
  
  # Get the argument count from the TU nodes
  my $fxnArgCnt = 0;
  if( defined $node->args )
  {
    my $arg = $node->args;
    while($arg)
    {
      $fxnArgCnt++;
    } continue {
      $arg = $arg->chain;
    }
  }
        
  # Let's check to see if the TU-based arg count matches the source-based
  # arg count
  if ($fxnArgCnt != scalar(@argListFromProcSrc))
  {
    # If the argList is actually just void, decrement arg count
    if (($fxnArgCnt == 1) && (scalar(@argListFromProcSrc) == 0))
    {
      $fxnArgCnt = 0;
    }
    elsif (($fxnArgCnt == 0) && (scalar(@argListFromProcSrc) == 1))
    {
      if ($argListFromProcSrc[0] eq "void")
      {
        pop @argListFromProcSrc;
      }
    }
    # Check to see if final argument from source-based list is a variadic placeholder
    elsif ($argListFromProcSrc[$#argListFromProcSrc] eq "...")
    {
      $fxn->isVariadic(1);
      die "Variadic functions not supported";
    }
    else
    {
      print "Argument count mismatch (",$fxnArgCnt,"!= ",scalar(@argListFromProcSrc),").\n";
      for (my $i = 0; $i < scalar(@argListFromProcSrc); $i++)
      {
        print "From source, argument $i =  ",$argListFromProcSrc[$i],"\n";
      }          
      die "Argument mismatch between Translation Unit and Source";
    }
  }
  
  if( defined $node->args )
  {
    my $arg = $node->args;
    for (my $i=0; $i < $fxnArgCnt; $i++)
    {
      $args[$i] = get_argument($arg);
      
      my $argName = $args[$i]->name;
      my $argText = $argListFromProcSrc[$i];
      $argText =~ s/(.*)$argName$/$1/g;
      $args[$i]->argTypeSourceString($argText);
      
      # Set the pointer type if appropriate
      if ($args[$i]->isPointer)
      {        
        if($argListFromOrigSrc[$i] =~ /^\s*(INBUF|OUTBUF|NONE|INOUTBUF|RAWBUF)\s+/)
        {
          $args[$i]->pointerType($1);
        }
      }    
      $arg = $arg->chain;
    }
  }
  
  #Set functions's argCnt and args list
  $fxn->argCnt($fxnArgCnt);
  
  $fxn->args(\@args);
}

sub get_includes_from_source($)
{
  my $cFile = shift or die "C filename not specified on the command line";
  my $includeString = "";
  
  # Find #include statements in the input c file and copy to output  
  open(C_SRC_FILE, '<',  $cFile) or die $!;
  while (<C_SRC_FILE>)
  {
    if (/^\s*#include\s+/)
    {
      my @fields = split();
      $includeString = $includeString . "#include $fields[1]\n";
    }
  }
  close(C_SRC_FILE);

  $includeString = $includeString . "\n";
  return $includeString;
}


# Functions for creating macros, externs, and includes for GPP2DSP direction
sub create_GPP_TO_DSP_stub_includes($$)
{
  my $cFile   = shift or die "C filename not specified";
  my $ppOpts  = shift;
  $ppOpts     = "" if not defined $ppOpts;
  my $includeString = "";
  
  $includeString = $includeString . "// Standard library required includes\n";
  $includeString = $includeString . "#include <stdlib.h>\n";
  $includeString = $includeString . "#include <stdio.h>\n";
  $includeString = $includeString . "#include <stdint.h>\n";
  $includeString = $includeString . "#include <stdbool.h>\n";
  $includeString = $includeString . "#include <string.h>\n\n";

  # Find #include statements to be placed in the output stub file
  $includeString = $includeString . "// Copied includes from original source file\n";

  my $pp_cFile = $cFile;
  $pp_cFile=~s/^(.*)\.(\w+)/$1\.pp_gpp_to_dsp\.$2/g;
  system("$ENV{ARM_TOOLCHAIN_PATH}/bin/$ENV{ARM_TOOLCHAIN_PREFIX}gcc -std=gnu99 -E -P -dI -Wno-unknown-pragmas $ppOpts -o $pp_cFile $cFile");

  my $ppIncludes    = get_includes_from_source($pp_cFile);
  my $origIncludes  = get_includes_from_source($cFile);
  my $finalIncludes = "";

  my @ppInc = split /\n/, $ppIncludes;
  my @origInc = split /\n/, $origIncludes;
  
  # Find all #include directives in the original source that still
  # exist in the pre-processed source
  for (my $i = 0; $i < scalar(@origInc); $i++)
  {
    for (my $j = 0; $j < scalar(@ppInc); $j++)
    {
      if ($origInc[$i] eq $ppInc[$j])
      {
        $finalIncludes = $finalIncludes . $origInc[$i] . "\n";
      }
    }
  }

  $includeString = $includeString . $finalIncludes;
  
  $includeString = $includeString . "\n#include \"gpp_to_dsp_stub.h\"\n";
  
  # Cleanup temporary pre-processed C file
  unlink $pp_cFile;  
  
  return $includeString;
}

sub create_DSP_FROM_GPP_stub_includes($$)
{
  my $cFile   = shift or die "C filename not specified";
  my $ppOpts  = shift;
  $ppOpts     = "" if not defined $ppOpts;
  my $includeString = "";
  
  $includeString = $includeString . "// Standard library required includes\n";
  $includeString = $includeString . "#include <stdlib.h>\n";
  $includeString = $includeString . "#include <stdio.h>\n";
  $includeString = $includeString . "#include <stdint.h>\n";
  $includeString = $includeString . "#include <stdbool.h>\n\n";

  # Find #include statements to be placed in the output stub file
  $includeString = $includeString . "// Copied includes from original source file\n";

  my $pp_cFile = $cFile;
  $pp_cFile=~s/^(.*)\.(\w+)/$1\.pp_dsp_from_gpp\.$2/g;
  system("$ENV{ARM_TOOLCHAIN_PATH}/bin/$ENV{ARM_TOOLCHAIN_PREFIX}gcc -std=gnu99 -E -P -dI -Wno-unknown-pragmas $ppOpts -o $pp_cFile $cFile");

  my $ppIncludes    = get_includes_from_source($pp_cFile);
  my $origIncludes  = get_includes_from_source($cFile);
  my $finalIncludes = "";

  my @ppInc = split /\n/, $ppIncludes;
  my @origInc = split /\n/, $origIncludes;
  
  # Find all #include directives in the original source that still
  # exist in the pre-processed source
  for (my $i = 0; $i < scalar(@origInc); $i++)
  {
    for (my $j = 0; $j < scalar(@ppInc); $j++)
    {
      if ($origInc[$i] eq $ppInc[$j])
      {
        $finalIncludes = $finalIncludes . $origInc[$i] . "\n";
      }
    }
  }

  $includeString = $includeString . $finalIncludes;

  $includeString = $includeString . "\n#include \"dsp_from_gpp_stub.h\"\n";  
  
  # Cleanup temporary pre-processed C file
  unlink($pp_cFile);
  
  return $includeString;  
}


# Functions for creating macros, externs, and includes for DSP2GPP direction
sub create_DSP_TO_GPP_stub_includes($$)
{
  my $includeString = "";
  return $includeString;
}

sub create_GPP_FROM_DSP_stub_includes($$)
{
  my $includeString = "";
  return $includeString;
}


# Functions for creating stubs for GPP2DSP direction
# GPP Calling (sending) stubs
sub create_GPP_TO_DSP_stub($)
{
  my $fxn             = shift or die "No function reference provided";
  
  my $fxnName         = $fxn->name;
  my $fxnRetType      = $fxn->returnType->string;
  my $fxnArgCnt       = $fxn->argCnt;
  
  my $fxnString       = "";
  my $macroSuffix     = "";
  
  
  # Add suffix for certain macros if the function does not return anything
  if ($fxnRetType eq "void")
  {
    $macroSuffix = "_VOIDRETURN";
  }
  
  # Start generating stub function text

  # ---------------------------------------------------------------------------------
  # Synchronous call
  # ---------------------------------------------------------------------------------  
  $fxnString = $fxnString       . "extern intptr_t FXN_ADDR_" . $fxnName . "_stub;\n";
  $fxnString = $fxnString       . "$fxnRetType $fxnName\n(\n";
  for(my $i=0; $i<$fxnArgCnt; $i++)
  {
    if ($i < ($fxnArgCnt - 1))
    {
      $fxnString = $fxnString   . "  " . $fxn->args->[$i]->argTypeSourceString . " " . $fxn->args->[$i]->name . ",\n";
    }
    else
    {
      $fxnString = $fxnString   . "  " . $fxn->args->[$i]->argTypeSourceString . " " . $fxn->args->[$i]->name . "\n";
    }
  }
  if ($fxnArgCnt == 0)
  {
    $fxnString = $fxnString     . "  void\n";
  }  
  $fxnString = $fxnString       . ")\n{\n";
  $fxnString = $fxnString       . "  RPC_INIT" . $macroSuffix . "(" . $fxnName . ", " . $fxnRetType . ");\n";

  # Output the cache operations for input buffers
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    my $argName = $fxn->args->[$i]->name;
    if ($fxn->args->[$i]->isPointer == 1)
    {
      my $pointerType = $fxn->args->[$i]->pointerType;
      if ($pointerType =~ /INBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_IN_OPS(" . $argName .", RPC_BUFFER_TYPE_INBUF);\n";
      }  
      elsif ($pointerType =~ /INOUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_IN_OPS(" . $argName .", RPC_BUFFER_TYPE_INOUTBUF);\n";
      }
    }
  }
  $fxnString = $fxnString       . "  RPC_TERMINATE_CACHE_OPS();\n";

  # Output the argument packing
  #FIXME - do param packing for pass-by-value structs, unions, etc.
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    my $argName = $fxn->args->[$i]->name;
    if ($argName ne 'void')
    {
      if ($fxn->args->[$i]->isPointer == 1)
      {
        my $pointerType = $fxn->args->[$i]->pointerType;
        if ($pointerType =~ /RAWBUF/)
        {
          $fxnString = $fxnString . "  RPC_PARAM_RAW_BUFFER_PACK(" . $argName .");\n";
        }
        else
        {  
          $fxnString = $fxnString . "  RPC_PARAM_BUFFER_PACK(" . $argName . ");\n";
        }        
      }
      else
      {
        $fxnString = $fxnString . "  RPC_PARAM_PACK(" . $argName . ");\n";
      }
    }
  }
  
  # Output the cache operations for output buffers
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
   my $argName = $fxn->args->[$i]->name;
    if ($fxn->args->[$i]->isPointer == 1)
    {
      my $pointerType = $fxn->args->[$i]->pointerType;
      if ($pointerType =~ /OUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_OUT_OPS(" . $argName .", RPC_BUFFER_TYPE_OUTBUF);\n";
      }
      elsif ($pointerType =~ /INOUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_OUT_OPS(" . $argName .", RPC_BUFFER_TYPE_INOUTBUF);\n";
      }
    }
  }
  $fxnString = $fxnString       . "  RPC_TERMINATE_CACHE_OPS();\n";  
  
  # Output the actual function dispatch call (sends message buffer, waits for return)
  $fxnString = $fxnString       . "  RPC_SYNC_DISPATCH();\n";
  
  # Unpack the result into result variable
  # FIXME - add support to return compound return types
  if ($fxnRetType ne 'void')
  {
    $fxnString = $fxnString     . "  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);\n";  
  } 
  # Return the result (waits for return message buffer)
  $fxnString = $fxnString       . "  RPC_RETURN" . $macroSuffix . "();\n"; 
  
  $fxnString = $fxnString       . "}\n";
  $fxnString = $fxnString       . "\n";
  # ---------------------------------------------------------------------------------  
  
  # ---------------------------------------------------------------------------------
  # Asynchronous Begin call
  # ---------------------------------------------------------------------------------
  $fxnString = $fxnString       . "C6RUN_RPC_AsyncHandle $fxnName" . "_asyncBegin\n(\n";
  for(my $i=0; $i<$fxnArgCnt; $i++)
  {
    if ($i < ($fxnArgCnt - 1))
    {
      $fxnString = $fxnString   . "  " . $fxn->args->[$i]->argTypeSourceString . " " . $fxn->args->[$i]->name . ",\n";
    }
    else
    {
      $fxnString = $fxnString   . "  " . $fxn->args->[$i]->argTypeSourceString . " " . $fxn->args->[$i]->name . "\n";
    }
  }
  if ($fxnArgCnt == 0)
  {
    $fxnString = $fxnString     . "  void\n";
  }
  $fxnString = $fxnString       . ")\n{\n";
  $fxnString = $fxnString       . "  RPC_INIT_ASYNC_BEGIN(" . $fxnName . ");\n";

  # Output the cache operations for input buffers
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    my $argName = $fxn->args->[$i]->name;
    if ($fxn->args->[$i]->isPointer == 1)
    {
      my $pointerType = $fxn->args->[$i]->pointerType;
      if ($pointerType =~ /INBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_IN_OPS(" . $argName .", RPC_BUFFER_TYPE_INBUF);\n";
      }  
      elsif ($pointerType =~ /INOUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_IN_OPS(" . $argName .", RPC_BUFFER_TYPE_INOUTBUF);\n";
      }
    }
  }
  $fxnString = $fxnString       . "  RPC_TERMINATE_CACHE_OPS();\n";

  # Output the argument packing
  #FIXME - do param packing for pass-by-value structs, unions, etc.
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    my $argName = $fxn->args->[$i]->name;
    if ($argName ne 'void')
    {
      if ($fxn->args->[$i]->isPointer == 1)
      {
        my $pointerType = $fxn->args->[$i]->pointerType;
        if ($pointerType =~ /RAWBUF/)
        {
          $fxnString = $fxnString . "  RPC_PARAM_RAW_BUFFER_PACK(" . $argName .");\n";
        }
        else
        {  
          $fxnString = $fxnString . "  RPC_PARAM_BUFFER_PACK(" . $argName . ");\n";
        }
      }
      else
      {
        $fxnString = $fxnString . "  RPC_PARAM_PACK(" . $argName . ");\n";
      }
    }
  }
  
  # Output the cache operations for output buffers
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
   my $argName = $fxn->args->[$i]->name;
    if ($fxn->args->[$i]->isPointer == 1)
    {
      my $pointerType = $fxn->args->[$i]->pointerType;
      if ($pointerType =~ /OUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_OUT_OPS(" . $argName .", RPC_BUFFER_TYPE_OUTBUF);\n";
      }
      elsif ($pointerType =~ /INOUTBUF/)
      {
        $fxnString = $fxnString . "  RPC_BUFFER_CACHE_OUT_OPS(" . $argName .", RPC_BUFFER_TYPE_INOUTBUF);\n";
      }
    }
  }
  $fxnString = $fxnString       . "  RPC_TERMINATE_CACHE_OPS();\n";  
  
  # Output the actual tx function dispatch call (sends message buffer)
  $fxnString = $fxnString       . "  RPC_ASYNC_DISPATCH_BEGIN();\n";
  
  $fxnString = $fxnString       . "}\n";
  $fxnString = $fxnString       . "\n";
  # ---------------------------------------------------------------------------------

  
  # ---------------------------------------------------------------------------------
  # Asynchronous isDone call
  # ---------------------------------------------------------------------------------
  $fxnString = $fxnString       . "bool $fxnName" . "_asyncIsDone\n(\n";
  $fxnString = $fxnString       . "  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__\n";
  $fxnString = $fxnString       . ")\n{\n";
  $fxnString = $fxnString       . "  return C6RUN_RPC_asyncIsComplete( __C6RUN_asyncHandle__ );\n";   
  $fxnString = $fxnString       . "}\n";
  $fxnString = $fxnString       . "\n";  
  # ---------------------------------------------------------------------------------

  
  # ---------------------------------------------------------------------------------
  # Asynchronous End call
  # ---------------------------------------------------------------------------------
  $fxnString = $fxnString       . "$fxnRetType $fxnName" . "_asyncEnd\n(\n";
  $fxnString = $fxnString       . "  C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__\n";
  $fxnString = $fxnString       . ")\n{\n";
  $fxnString = $fxnString       . "  RPC_INIT_ASYNC_END" . $macroSuffix . "(" . $fxnRetType . ");\n";

  # Output the actual rx function dispatch call (waits for return)
  $fxnString = $fxnString       . "  RPC_ASYNC_DISPATCH_END();\n";
  
  # Unpack the result into result variable
  # FIXME - add support to return compound return types
  if ($fxnRetType ne 'void')
  {
    $fxnString = $fxnString     . "  RPC_RESULT_UNPACK(__C6RUN_fxnResult__);\n";  
  } 
  # Return the result (waits for return message buffer)
  $fxnString = $fxnString       . "  RPC_RETURN" . $macroSuffix . "();\n"; 
  
  $fxnString = $fxnString       . "}\n";
  $fxnString = $fxnString       . "\n";
  # ---------------------------------------------------------------------------------


  # End stub function body generation
  return $fxnString;
}

# DSP Callee (receiving) stubs
sub create_DSP_FROM_GPP_stub($)
{
  my $fxn             = shift or die "No function reference provided";
  
  my $fxnName         = $fxn->name;
  my $fxnRetType      = $fxn->returnType->string;
  my $fxnArgCnt       = $fxn->argCnt;
  
  my $fxnString       = "";
  my $macroSuffix     = "";

  my @argNames;

  # For void return types, certain RPC macros are named with the suffix 
  if ($fxnRetType eq "void")
  {
    $macroSuffix = "_VOIDRETURN";
  }  

  # Start generating stub function text
  $fxnString = $fxnString     . "#if defined(__TI_EABI__)\n";
  $fxnString = $fxnString     . "  #define " . $fxnName . "_stub _" . $fxnName . "_stub\n";
  $fxnString = $fxnString     . "#endif\n";  
  $fxnString = $fxnString     . "void $fxnName" . "_stub( void *msgBuffer )\n";
  $fxnString = $fxnString     . "{\n";

  # Issue the INIT macro
  $fxnString = $fxnString     . "  RPC_INIT" . $macroSuffix . "( " . $fxnRetType . " );\n";
  
  # Always process input cache ops (may be nothing but a null terminator)
  $fxnString = $fxnString     . "  RPC_PROCESS_CACHE_IN_OPS();\n";

  # Open the section of code dealing with the function arguments and function call
  $fxnString = $fxnString     . "  {\n";
  
  # Create variables for function input arguments
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    my $argTypeString = $fxn->args->[$i]->argTypeSourceString;
    $argNames[$i] = "arg" . $i;
    $fxnString = $fxnString   . "    RPC_ARG_DECLARATION(" . $argNames[$i] . ", " . $argTypeString . ");\n";
  }
  
  # Output the argument unpacking
  for(my $i=0;$i < $fxnArgCnt; $i++)
  {
    $fxnString = $fxnString   . "    RPC_PARAM_UNPACK(" . $argNames[$i] . ");\n";
  }
  
  # Issue the remote procedure call with proper number of arguments
  my $paramText = join ',',@argNames;
  if ($fxnRetType ne 'void')
  {
    $fxnString = $fxnString   . "    __C6RUN_fxnResult__ = " . $fxnName ."( " . $paramText . ");\n";
  }
  else
  {
    $fxnString = $fxnString   . "    " . $fxnName ."( " . $paramText . ");\n";
  }
  
  # Close the section of code dealing with the function arguments and function call
  $fxnString = $fxnString     . "  }\n";
  
  # Always process output cache ops (may be nothing but a null terminator)
  $fxnString = $fxnString     . "  RPC_PROCESS_CACHE_OUT_OPS();\n";
  
  # Unpack the result into result variable
  # FIXME - handle packing return-by-value struct results (or other compound types)
  if ($fxnRetType ne 'void')
  {
    $fxnString = $fxnString   . "  RPC_RESULT_PACK(__C6RUN_fxnResult__);\n";
  }
  
  # Terminate function stub
  $fxnString = $fxnString     . "}\n\n";

  # End stub function body generation
  return $fxnString;
}

# Functions for creating stubs for DSP2GPP direction
# DSP Calling (sending) stubs
sub create_DSP_TO_GPP_stub($)
{

}

# GPP Callee (receiving) stubs
sub create_GPP_FROM_DSP_stub($)
{

}



###############################################################################
# Get the input arguments to this script                                      #
###############################################################################
# 
# inputCFile   = source C file which will be analyzed for stub generation
# direction   = GPP2DSP or DSP2GPP (which way the RPCs are going)
my $inputCFile      = shift or die "C filename not specified on the command line";
my $outputObjFile   = shift or die "Output object filename not specified on the command line";
my $direction       = shift or die "Mode/direction not specified on the commandline";
my $debug           = shift;

# Validate direction options
$direction =~ tr/a-z/A-Z/;
if (($direction ne "GPP2DSP") && ($direction ne "DSP2GPP"))
{
  die("Invalid direction parameter");
}

# Validate debug options (if one was given)
my $DEBUG = 0;
if (defined $debug)
{
  $debug =~ tr/a-z/A-Z/;
  if ($debug eq "RELEASE")
  {
    $DEBUG = 0
  }
  elsif ($debug eq "DEBUG")
  {
    $DEBUG = 1
  }
  else
  {
    die("Invalid debug/release parameter");
  }
}

# Get output path
my $outPath  = $outputObjFile;
$outPath=~s/^(.*\/)*(.*)\.(\w+)/$1/g;

# Get input source file extension
my $inExt = $inputCFile;
$inExt=~s/^(.*\/)*(.*)\.(\w+)/$3/g;

my $gpp_stub_file = $outputObjFile;
my $dsp_stub_file = $outputObjFile;
my $fxn_list_file = $outputObjFile;
if (defined $outPath)
{
  $gpp_stub_file=~s/^(.*\/)*(.*)\.(\w+)/$outPath$2\.$3/g;
  $dsp_stub_file=~s/^(.*\/)*(.*)\.(\w+)/$outPath$2\.$3/g;
  $fxn_list_file=~s/^(.*\/)*(.*)\.(\w+)/$outPath$2\.$3/g;
}

# Create output stub and list filenames
$gpp_stub_file=~s/^(.*)\.\w+/$1\.gpp_stub\.$inExt/g;
$dsp_stub_file=~s/^(.*)\.\w+/$1\.dsp_stub\.$inExt/g;
$fxn_list_file=~s/^(.*)\.\w+/$1\.fxn_list\.txt/g;

# Open the the output files for writing
open(FXN_LIST_FILE, '>', $fxn_list_file) or die $!;
open(GPP_STUB_FILE, '>', $gpp_stub_file) or die $!;
open(DSP_STUB_FILE, '>', $dsp_stub_file) or die $!;


###############################################################################
# Begin script main processing here                                           #
###############################################################################

# Step 0 - Set and generate appropriate source filenames
function->setSourceFileNames($inputCFile,$outPath);
my $inputCFileShort         = function->inputSourceFileShort();
my $inputTuFile             = $inputCFileShort . ".001t.tu";

my $preprocessedCFile       = function->preprocessedSourceFile();
my $preprocessedCFileShort  = function->preprocessedSourceFileShort();
my $preprocessedTuFile      = $preprocessedCFileShort . ".001t.tu";


# Step 1 - Get preprocessor options that could affect code analysis
my $ppOpts = "";
if ( -e "$ENV{C6RUN_INSTALL_DIR}/lib/perl/pp_opts.txt" )
{
  open(PP_OPTS_FILE, '<', "$ENV{C6RUN_INSTALL_DIR}/lib/perl/pp_opts.txt") or die $!;
  while(<PP_OPTS_FILE>)
  {
    $ppOpts = $ppOpts . $_;
  }
  close(PP_OPTS_FILE);
}


# Step 2 - Pre-process the input C file to get a pre-processed C file that we can compare against.
#   We do this for a few reasons, the primary ones being that we need to get to pure C code (no PP
#   macros to interfere with our source analysis) and to remove conditional statements (which also 
#   could interfere with source analysis)
my $result;
$result = system("$ENV{ARM_TOOLCHAIN_PATH}/bin/$ENV{ARM_TOOLCHAIN_PREFIX}gcc -std=gnu99 -E -P -Wno-unknown-pragmas $ppOpts -o $preprocessedCFile $inputCFile");


# Step 3 - Compile the original C and the pre-processed C to get GCC translation unit.
#   Note that we include the c6x_compat.h in order to satisfy possible C6000 intrinsic 
#   usage that GCC wouldn't otherwise understand.
$result = system("$ENV{ARM_TOOLCHAIN_PATH}/bin/$ENV{ARM_TOOLCHAIN_PREFIX}gcc -std=gnu99 -Wno-unknown-pragmas -fdump-translation-unit -include $ENV{C6RUN_INSTALL_DIR}/include/c6run/gpp/c6x_compat.h $ppOpts -c -o _temp_.o $inputCFile");
$result = system("$ENV{ARM_TOOLCHAIN_PATH}/bin/$ENV{ARM_TOOLCHAIN_PREFIX}gcc -std=gnu99 -Wno-unknown-pragmas -fdump-translation-unit -include $ENV{C6RUN_INSTALL_DIR}/include/c6run/gpp/c6x_compat.h $ppOpts -c -o _temp_.o $preprocessedCFile");
#   Delete the temporary output file
unlink "_temp_.o";
#   TU files are now in the local directory


# Step 4 - Parse the translation unit info of the original C source. We do this to find the
#   names of the functions that we will analyze and work with for generating remote procedure 
#   call stubs
my $node = GCC::TranslationUnit::Parser->parsefile($inputTuFile)->root;


# Step 5 - Find all global function names from the original C source file
while($node)
{
  # Cycle through the nodes of the translation unit and
  # only look at ones that are function declarations...
  if($node->isa('GCC::Node::function_decl'))
  {
    # ... from the current source file
    if( (split /:/,$node->source)[0] eq $inputCFileShort)
    {
      # ... and defined in the pre-processed source file
      if ($node->body ne "undefined" )
      {
        # Set function's name and original source line
        new function(
          name            => $node->name->identifier,
          origSourceLine  => (split /:/,$node->source)[1]
        );
      }
      elsif ($DEBUG)
      {
        print "Function " . $node->name->identifier . " is declared but not defined.\n";
      }      
    }
  }
} continue {
  $node = $node->chain;
}


# Step 6 - Begin output of the stub C files, in particular the #include, extern and
#   macro declarations.  This should be everything except the actual stub function
#   definitions themselves.
my $gppIncludeString;
my $dspIncludeString;
if ($direction eq "GPP2DSP")
{
  $gppIncludeString = create_GPP_TO_DSP_stub_includes($inputCFile, $ppOpts);
  $dspIncludeString = create_DSP_FROM_GPP_stub_includes($inputCFile, $ppOpts);
}
else
{
  $dspIncludeString = create_DSP_TO_GPP_stub_includes($inputCFile, $ppOpts);
  $gppIncludeString = create_GPP_FROM_DSP_stub_includes($inputCFile, $ppOpts);

}
# Insert header file include statements for each stub
print GPP_STUB_FILE "$gppIncludeString";
print DSP_STUB_FILE "$dspIncludeString";


# Step 7 - Parse the translation unit info of the pre-processed C source. We use this file
# as the actualy basis for our source analysis, since it has no pre-processor elements that
# might confuse us.
my $node = GCC::TranslationUnit::Parser->parsefile($preprocessedTuFile)->root;


# Step 8 - Create the stub function definitions using the information that we collected
#   in all the previous steps.  We mostly collect the info we need from the pre-processed C
#   source.
while($node)
{
  # Cycle through the nodes of the translation unit and
  # only look at ones that are function declarations...
  if($node->isa('GCC::Node::function_decl'))
  {
    # ...from the current source file
    if( (split /:/,$node->source)[0] eq $preprocessedCFileShort)
    {
      # Get the function's name
      my $fxnName = $node->name->identifier;
      
      # Check to see if this function name is one we care about ...
      my $fxn = function->findFunction($fxnName);
      if (defined($fxn))
      {
        # Print the RPC'd function names to a list in a txt file for use
        # in other parts of the build system.
        print FXN_LIST_FILE "$fxnName\n";
        
        # Set function TU node
        $fxn->tuNode($node);
        
        # Set function's pre-processed source line
        $fxn->procSourceLine( (split /:/,$node->source)[1] );
        
        # Set function return type
        my $fxnRetType = new type(
          tuNode  => $node->type->retn
        );
        $fxn->returnType($fxnRetType);
        
        # Get the function arguments
        update_fxn_args($fxn);
        if ($DEBUG == 1)
        {
          $fxn->print();
        }
        
        # Generate and output the stub functions
        my $gppStubFxnString;
        my $dspStubFxnString;
        if ($direction eq "GPP2DSP")
        {
          $gppStubFxnString = create_GPP_TO_DSP_stub($fxn);
          $dspStubFxnString = create_DSP_FROM_GPP_stub($fxn);
        }
        else
        {
          $gppStubFxnString = create_GPP_FROM_DSP_stub($fxn);
          $dspStubFxnString = create_DSP_TO_GPP_stub($fxn);
        }
        #Insert the stub function text into the output stub files
        print GPP_STUB_FILE "$gppStubFxnString";
        print DSP_STUB_FILE "$dspStubFxnString";
      }
    }
  }
} continue {
  $node = $node->chain;
}

# Add final newline to generated stubs C file
print GPP_STUB_FILE "\n";
print DSP_STUB_FILE "\n";

# Clean up temporary files
if ($DEBUG != 1)
{
  unlink($preprocessedCFile);
  unlink($inputTuFile);
  unlink($preprocessedTuFile);
}

# Close up all opened files
close(GPP_STUB_FILE);
close(DSP_STUB_FILE);
close(FXN_LIST_FILE);

