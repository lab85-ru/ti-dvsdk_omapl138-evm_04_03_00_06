TMS320C6000 C/C++ CODE GENERATION TOOLS
6.1.14 Release Notes
March 2010

===============================================================================
Release Notes
===============================================================================

1. Defect History

The list of defects fixed in this release as well as known issues can
be found in the file DefectHistory.txt.

2. Compiler Documentation Errata

Errata from the "TMS320C6000 Optimizing Compiler User's Guide" and the 
"TMS320C6000 Assembly Language User's Guide" is available online at the
Texas Instruments Embedded Processors CG Wiki:

http://tiexpressdsp.com/wiki/index.php?title=Category:CGT

under the 'Compiler Documentation Errata' link.

This Wiki has been established to assist developers in using TI Embedded
Processor Software and Tools.  Developers are encouraged to dig through all the
articles.  Registered users can update missing or incorrect information.

3. C67x Fast Run-Time Support (RTS) Library

These libraries are no longer included in the C6000 Code Generation Tools.  
They are available for download at the following Texas Instruments page:

http://focus.ti.com/docs/toolsw/folders/print/sprc060.html

Or search the ti.com site for the tag: tms320c67x fastrts library

4. Development Tools Forum

For development tools support please see the Texas Instruments 
engineer-to-engineer (E2E) Community and the Development Tools
Forum at:

http://e2e.ti.com/support/development_tools/default.aspx


===============================================================================
New Features in this release:
===============================================================================

- Profile directed optimization 
- Compiler based Code Coverage Analysis
- New COFF Linker
- Support for the C674x ISA
- New C++ template library support in the RTS library
- Function entry/exit hooks
- Improvements to conditional linking of compiled C/C++ code
- Addition of the FUNC_ALWAYS_INLINE pragma
- Improved support for "restrict" qualifier
- Call-site based inlining
- Structure access optimizations
- GCC Language extensions
- SIMD optimizations


===============================================================================
Contents
===============================================================================
 1) New Profiling Infrastructure
    1.1) Profile Directed Optimization
    1.2) Profile Data Collection
    1.3) Profile Data Decoder
    1.4) Profile Restrictions and Notes
    1.5) Profile Based Code Coverage
    1.6) Profile API
    1.7) Profile Summary
 2) Changes to the Runtime Support Libraries
 3) C++ Standard Library Support
 4) GCC Language Extensions
 5) Function Entry/Exit Hooks
 6) Improved Support for "restrict" Qualifier
 7) Support for the c674x ISA
 8) Conditional Linking Changes When Compiling C/C++ Code
 9) The New FUNC_ALWAYS_INLINE pragma
10) Call Site Based Inlining
11) New COFF Linker 


-------------------------------------------------------------------------------
1) New Profiling Infrastructure
-------------------------------------------------------------------------------

This release supports a new method for finding the frequently executed paths 
in an application using compiler-based instrumentation.  This information 
is then fed back to the compiler and is used to perform optimizations and 
provide the user with information about application behavior. The new 
technique is called path-profiling.

-------------------------------------------------------------------------------
1.1) Profile Directed Optimization
-------------------------------------------------------------------------------
Profile directed optimization uses run-time profile feedback to identify and 
optimize frequently executed program paths.  Profile directed optimization 
is accomplished using the following steps:

1. Instrument the program for profiling
2. Execute the profiled program
3. Write the profile data to a file
4. Consolidate profile data into a profile feedback file
5. Re-compile the program using the profile feedback file

Step 1: Instrument the program for profiling
--------------------------------------------
The compiler is invoked with the option --gen_profile_info, which directs the
compiler to insert a minimal amount of instrumentation code to determine 
control flow frequencies.  In addition, memory is allocated for profiling
counters.  For example:

cl6x -mv6400+ -o --gen_profile_info foo.c -z -o a.out -llnk.cmd -lrts64plus.lib

Step 2: Execute the profiled program
------------------------------------
The instrumented program is executed on the target using representative 
input data sets. The input data sets should correlate closely
with the way the program is expected to be used in the end product
environment. 

Step 3: Write the profile data to a file
----------------------------------------
The program must complete and call a routine to write the collected
profile data into a file.  The Run Time Support (RTS) function exit()
has built-in support for writing profile data into a profile data
file.  Multiple executions of the program using different input
data can append to the profile data file.

Step 4: Consolidate profile data into a profile feedback file
-------------------------------------------------------------
After all the input data sets have been executed using the profiled program,
the profile data file is post-processed using a tool called the 'profile 
data decoder' or pdd6x.  The pdd6x tool will consolidate multiple data sets 
and format the profile data.  The pdd6x tool will create a new file 
called the profile feedback file.  For example:

pdd6x -e foo.out -o f.prf pprofout.pdat

Step 5: Re-compile the program using the profile feedback file
--------------------------------------------------------------
The compiler is invoked with the --use_profile_info=<file.prf> option, 
which directs the compiler to use the profile feedback file to guide program
optimization.  The profile feedback is used to guide optimizations such 
as function inlining, loop unrolling, case statement ordering, instruction 
scheduling, and register allocation. For example:

cl6x -mv6400+ -o --use_profile_info=f.prf foo.c -z -o a.out -llnk.cmd 
-lrts64plus.lib

-------------------------------------------------------------------------------
1.2) Profile Data Collection
-------------------------------------------------------------------------------
The profile data is collected by inserting instrumentation code at control 
flow points in the program.  The instrumentation code increments counters
that are stored in memory.  At some point the profile counters must be written
from memory to a profile data file.  The exit() function handles writing the 
profile counters to a profile data file.  By default, the RTS C entry point
routine in boot.c will invoke the exit() function when the main() function 
completes.  Another option is to use the profile API discussed in section 
1.6 to control when the counters are written to the profile data file.

The .ppdata Section
-------------------
The profile data is stored in the ".ppdata" section, which must be 
allocated in memory.  The .ppdata section contains the profile counters 
for all functions compiled with --gen_profile_info.

The default lnk.cmd file bundled in the release has directives to place 
the .ppdata section in data memory.  If the linker command file has no 
section directives for allocating the .ppdata section, the linker will 
place it in a writable memory range. 

Writing Profile Data Files
--------------------------
By default, the RTS profile data output routine will use the low-level
I/O functions to write the profile data file (See Section 9 in the Optimizing
Compiler User's Guide).  A device handler called "PPHNDL" is supported for 
writing profile data to a custom device driver routine. 

Instrumented Program Execution Overhead
---------------------------------------
During program profiling, the execution time of the application will most
likely increase due to the overhead of updating the profiling counters.

The profiling counters increase the code and data size of the program.  
Consider using the -ms code size options when using profiling to 
mitigate the code size increase.  This will have no effect on the
accuracy of the profile data being collected.


-------------------------------------------------------------------------------
1.3) Profile Data Decoder (pdd6x)
-------------------------------------------------------------------------------
The code generation tools include a new tool called the profile data decoder 
or pdd6x, which is used for post processing profile data files. The pdd6x 
tool generates a profile feedback file. See section 1.1 for a discussion on 
where pdd6x fits in the profiling flow.  The pdd6x tool is invoked as follows:

pdd6x -e exec.out -o exec.prf pprofout.pdat

exec.out - the name of the program executable.

exec.prf - the formatted profile feedback file that will be used as
           the argument to --use_profile_info.

pprofout.pdat - the name of one or more profile data files. 

If no output file is specified (i.e., there is no -o option), the default
profile feedback filename is pprofout.prf.

The pdd6x tool will append to the profile feedback file and will not
over-write them. This enables the accumulation of data from multiple
profile runs.

The pdd6x tool, by default, will accumulate (i.e., compute the sum of) profile
data values from multiple datasets corresponding to the same function. The
command-line option --average can be used to instruct pdd6x to compute the
average of data values from all datasets instead of accumulating them.

-------------------------------------------------------------------------------
1.4) Profiling Restrictions and Notes
-------------------------------------------------------------------------------

Profile-Directed Optimization and Code Size Tune
-----------------------------------------------
Profile directed optimization is different from Code Size Tune found 
in CCS.  The Code Size Tune feature in CCS uses profiling to select specific
compilation options for each function in order to minimize code size while 
still maintaining a specific performance point.  Code Size Tune is 
coarse-grain, since it is selecting an option set for the whole function.  
Profile Directed Optimization selects different optimization goals along 
specific regions within a function.

Invalid Profile Feedback File
-----------------------------
When compiling with --use_profile_info=<file.prf>, the profile feedback file 
will be invalid in the following cases:

1. The source code or file name has been changed since the last time the 
   file was compiled with the --gen_profile_info option.

2. Compiler options used with --gen_profile_info are different from
   those used with --use_profile_info. In particular, options that change
   parser behavior could invalidate a profile feedback file. 

NOTE: It is safe to compile a program with different optimization and code
size options between --gen_profile_info and --use_profile_info.

NOTE: The application must be compiled with at-least skeletal (dwarf) debug
support to enable profiling. The pdd6x tool relies on basic debug information
about each function compiled for profiling and its profile counter
characteristics to be able to generate the formatted .prf file

NOTE: The profile data file generated by the RTS is a raw data file of a
fixed format understood only by the pdd6x.  Do not modify this file.


-------------------------------------------------------------------------------
1.5) Profile Based Code Coverage
-------------------------------------------------------------------------------
The information collected during program profiling can also be used for
generating code coverage reports.  As with profile directed optimization,
the program must be compiled with the --gen_profile_info option.  Code 
coverage reports are generated using the following steps:

1. Instrument the program for profiling
2. Execute the profiled program
3. Write the profile data to a file
4. Consolidate profile data into a profile feedback file
5. Generate code coverage report

Steps 1-4 are exactly the same as those described in section 1.1.

Step 5: Generate Code Coverage Report
-------------------------------------
The compiler is invoked with the --use_profile_info=<file.prf> option
and either the --codecov or --onlycodecov option.  The compiler will generate 
a code coverage file.  For example, the following command line will result
in the code coverage file foo.csv.

   cl6x -o2 --use_profile_info=pprofout.prf --onlycodecov foo.c

Users can specify two environment variables to control the destination of 
the code coverage file

1. The TI_COVDIR environment variable can be used to specify the directory 
   where the code coverage file should be generated. Default is the directory 
   where the compiler is invoked

2. The TI_COVDATA environment variable can be used to specify the name of the 
   code coverage file generated by the compiler. The default is "filename.csv",
   where filename is the base-name of the file being compiled.  For example, 
   if foo.c is being compiled, the default code-coverage file is foo.csv

If the code coverage file already exists, the compiler will append the 
new dataset at the end of the file. 

Code coverage data is a comma-separated list of data items that can be 
conveniently handled by data-processing tools and scripting languages. The 
following is the format of code-coverage data:

"filename-with-full-path","funcname",line#,column#,exec-frequency,"comments"

filename-with-full-path: Full pathname of the file corresponding to the entry
funcname: Name of the function 
line#: Line number of the source line corresponding to frequency data
column#: Column number of the source line 
exec-frequency: Execution frequency of the line
comments: Intermediate-level representation of the source-code generated by
the parser.

Note that full-filename, function name and comments appear within quotes (").

E.g.
"/some_dir/zlib/c64p/deflate.c","_deflateInit2_",216,5,1,"( strm->zalloc ) "

Other tools, such as a spreadsheet program,  can be used to format and view 
the code coverage data.

-------------------------------------------------------------------------------
1.6) Profile API
-------------------------------------------------------------------------------
The following are application programmer interfaces to the profiler 
mechanism for collecting and writing out profile data. They can be used
to control the starting and stopping of profiling and when profile data
is written to a file.

1. _TI_start_pprof_collection() - Start profiling and clear all profile 
                                  counters.

2. _TI_stop_pprof_collection()  - Write profile counters to a profile data
                                  file and prevent the exit() routine from
                                  writing out profile counters unless
                                  _TI_start_pprof_collection() is called
                                  again. 

NOTE: Every call to _TI_stop_pprof_collection() API will cause the RTS to append
profile data for the application to the profile data file as a new dataset. 
   
-------------------------------------------------------------------------------
1.7) Profile Summary
-------------------------------------------------------------------------------

Options Descriptions
---------------------
--gen_profile_info 
    This option directs the compiler to add instrumentation code to collect 
    profile information.  When the program executes the RTS exit() 
    function, the profile data is written to a profile data file.  The
    name of the profile data file can be specified in the TI_PROFDATA
    environment variable or it defaults to pprofout.pdat.

--use_profile_info=[file1,file2,...,filen]
    This option specifies the profile feedback file(s) to use for profile
    directed optimization.  The compiler will consolidate data from multiple
    feedback files.

--codecov
  Generate code coverage information file and continue with profile based 
  optimization.  Must be used with --use_profile_info.

--onlycodecov 
  Same as --codecov, but only generates code coverage data files.

Environment Variables
---------------------
TI_PROFDATA - Write profile data into specified file
TI_COVDIR   - Create code coverage files in specified directory
TI_COVDATA  - Write code coverage data into specified file

API
---
_TI_start_pprof_collection() - clear profile counters
_TI_stop_pprof_collection()  - dump profile counters 
PPHDNL - device driver handle for low-level I/O driver for writing
         out profile data from a target program.

Files Created
-------------
*.pdat  - Profile DATa file, created by executing a profiled program,
          input to the profile data decoder (pdd6x)
*.prf   - PRofile Feedback file, created by the profile data decoder,
          input to the re-compilation step.

pdd6x Options
-------------
-e execfile 
 Name of the executable file used to generate the profile data file. Must be a
 valid executable file.

-o filename.prf
 Name of the profile feedback file into which pdd6x writes/appends its output.

-a
 Compute the average of the data values in the datasets instead of accumulating
 data values.

filename 
 Name of the profile data file created by executing a profiled program 
 (e.g., pprofout.pdat). Multiple profile data filenames can be specified on the 
 command-line.

-------------------------------------------------------------------------------
2) Changes to the Runtime Support Libraries
-------------------------------------------------------------------------------

The following changes and additions apply to the runtime support libraries in
the /lib subdirectory of the release package.

Minimal Support for Internationalization
----------------------------------------
The library now includes the header files <locale.h>, <wchar.h>, and
<wctype.h>, which provide APIs to support non-ASCII character sets and
conventions. Our implementation of these APIs is limited in the following
ways:

1. The library has minimal support for wide and multi-byte characters. The 
   type wchar_t is implemented as 'int'. The wide character set is equivalent 
   to the set of values of type 'char'. The library includes the header 
   files <wchar.h> and <wctype.h> but does not include all the functions 
   specified in the standard. So-called multi-byte characters are limited to 
   single characters. There are no shift states. The mapping between multi-byte
   characters and wide characters is simple equivalence; that is, each wide 
   character maps to and from exactly a single multi-byte character having
   the same value.

2. The C library includes the header file <locale.h> but with a minimal
   implementation. The only supported locale is the "C" locale. That is, 
   library behavior that is specified to vary by locale is hard-coded to 
   the behavior of the "C" locale, and attempting to install a different 
   locale via a call to "setlocale()" will return NULL.

Allowable Number of Open Files
------------------------------
In the <stdio.h> header file, the value for the macro FOPEN_MAX has been
changed from 12 to the value of the macro _NFILE, which is set to 10.  The
impact is that you can only have 10 files simultaneously open at one time
(including the pre-defined streams - stdin, stdout, stderr).

The C standard requires that the minimum value for the FOPEN_MAX macro is 8.
The macro determines the maximum number of files that can be opened at one
time. The macro is defined in the stdio.h header file and can be modified by
changing the value of the _NFILE macro.

Accessing Library Source and Building Libraries
-----------------------------------------------
Instead of including an rts.src file, we now ship an rtssrc.zip file. When
extracted (with any standard unzip tool on windows, linux, or unix), this zip
file will recreate the runtime source tree for run-time library.  The zip file
also includes the Makefiles and perl scripts needed to rebuild the library.  
By simply modifying the definitions of the option bases in the target 
Makefile (or even adding new definitions), you simply use gmake to rebuild 
the run-time library.  You must have access to any perl after version 5.6 
and a recent gmake.  Short instructions are in the Makefile.

Simple instructions: 

1. Unzip rtssrc.zip 

2. Modify OPT_XXX Makefile variable, or add a new OPT_XXX base if changing 
   options used to build a library.

3. Or edit the necessary rts source file.  For example, you may need to edit 
   the start-up routine, boot.asm, for your device.

4. Call "gmake" to rebuild the library.

Automatic Library Selection
---------------------------
The linker now has the capability to automatically choose the correct library
to match the target and model options of your object code. Refer to the
companion file LINKER_README.txt for details on this useful feature.

-------------------------------------------------------------------------------
3) C++ Standard Library Support
-------------------------------------------------------------------------------

The archive libraries in the /lib subdirectory now include support for the 
complete C++ Standard Library.  The library includes all the features of the 
C++ Library, including the Standard Template Library (STL), streams, and 
strings. The following exceptions and caveats are noted: 

 - The C++ library supports wide chars, in that template functions and classes 
   that are defined for char are also available for wide char. For example 
   wide char stream classes wios, wiostream, wstreambuf and so on 
   (corresponding to char classes ios, iostream, streambuf) are implemented. 
   However there is no low-level file I/O for wide chars. Also, the C library 
   interface to wide char support (via the C++ headers <cwchar> and 
   <cwctype>) is limited as described above under "Changes to the Runtime
   Support Libraries."

Our C++ library is licensed from Dinkumware, Ltd (www.dinkumware.com). 
The Dinkumware C++ library is a fully conforming, industry-leading 
implementation of the standard C++ library.

TI does not provide documentation that covers the functionality of the C++
library. We suggest referring to one of the following sources:

   - The Standard C++ Library: A Tutorial and Reference, Nicolai M. Josuttis,
     Addison-Wesley, ISBN 0-201-37926-0 
   - The C++ Programming Language (Third or Special Editions), Bjarne 
     Stroustrup, Addison-Wesley, ISBN 0-201-88954-4 or 0-201-70073-5. 
   - Dinkumware's online reference at http://dinkumware.com/manuals/ 

-------------------------------------------------------------------------------
4) GCC Language Extensions
-------------------------------------------------------------------------------

The GNU compiler, GCC, provides a number of language features not found in the
ANSI standard C. The definition and official examples of these extensions can
be found online at

http://gcc.gnu.org/onlinedocs/gcc-3.4.4/gcc/C-Extensions.html#C-Extensions

The --gcc option enables support for the GCC extensions described below.

The extensions that the TI compiler supports are listed in the following table.

Extension              Description
---------              -----------
Statement Expressions  Placing statements and declarations inside expressions
                          (useful for creating smart "safe" macros).
Local Labels           Labels local to a statement-expression.
Naming Types           Giving a name to the type of some expression.
Typeof Operator        typeof referring to the type of an expression
Generalized Lvalues    Using '? ', ',' and casts in lvalues

Conditionals           Omitting the middle operand of a '? ' expression
Long Long              Double-longword integers - long long int.
Hex Floats             Hexadecimal floating-point constants.
Zero Length            Zero-length arrays.
Macro Varargs          Macros with variable number of arguments.
Subscripting           Any array can be subscripted, even if not an lvalue.

Pointer Arithmetics    Arithmetic on void-pointers and function pointers.
Initializers           Non-constant initializers.
Cast Constructors      Constructor expressions give structures, unions or
                           arrays as values.
Labeled Elements       Labeling elements of initializers.
Cast to Union          Casting to union type from any member of the union.
                           Case Ranges 'case 1 ... 9' and such.
Function Attributes    Declaring that functions have no side effects, or that
                           they can never return.
Function Prototypes    Prototype declarations and old-style definitions.


C++ Comments           C++ comments are recognized.
Dollar Signs           Dollar sign is allowed in identifiers.
Character Escapes      '\e' stands for the character ESC.
Alignment              Inquiring about the alignment of a type or variable.
Variable Attributes    Specifying attributes of variables.
Type Attributes        Specifying attributes of types.
Inline Defining        inline functions (as fast as macros).
Asm Labels             Specifying the assembler name to use for a C symbol.
Alternate Keywords     __const__, __asm__, etc., for header files.
Incomplete Enums       enum foo;, with details to follow.
Function Names         Printable strings which are the name of the current
                          function.
Return Address         Getting the return or frame address of a function.
                          __builtin_return_address, __builtin_frame_address
Other Builtins         Other built-in functions.__builtin_constant_p,
                          __builtin_expect

Function attributes

The GNU extension support provides a number of attributes about functions to
help the compiler's optimization.  The TI compiler will accept only 3 of these 
attributes.  All other attributes are ignored.  At this time, no warning or 
error is given for ignored attributes.  The following table lists the 
attributes that are supported.

Attribute    Description
---------    -----------
deprecated   This function exists but the compiler must generate a warning
             if it is used.
section      Place this function in the specified section.
unused       The function is meant to be possibly unused.


Builtin Functions

The TI tools only provide support for the following four built-in functions:

__builtin_constant_p(expr)
   Returns TRUE only if expr is a constant at compile time.

__builtin_expect(expr, CONST)
   Returns expr. The compiler will use this to optimize along paths
   determined by conditional statements such as if-else.  While this
   builtin can be used anywhere in your code, it will only convey
   useful information to the compiler if it is the entire predicate
   of an if-statement and CONST is 0 or 1.  For example:

   if (__builtin_expect(a == 3, 1)) 

   indicates that the user expects the predicate "a == 3" to be
   true most of the time.

__builtin_return_address(int level)
    Returns 0.

__builtin_frame_address(int level)
    Returns 0.

-------------------------------------------------------------------------------
5) Function Entry/Exit Hooks
-------------------------------------------------------------------------------

An entry hook is a routine that is called upon entry to each function in
the program. An exit hook is a routine that is called upon exit of each 
function. Applications for hooks include debugging, trace, profiling,
and stack overflow checking.

Entry hooks are enabled using the following options:

   --entry_hook[=<name>] 
      Enable entry hooks. The hook function is called <name>. If no name
      is given, the default entry hook  function name is __entry_hook.

   --entry_parm=(name | address | none)
      Specify the parameters to the hook function. The option 
      '--entry_parm=name' specifies that the name of the calling function 
      is passed to the hook function as an argument. In this case the 
      signature for the hook function is:
         void hook(const char *name);

      The option '--entry_parm=address' specifies that the address of the 
      calling function is passed to the hook function. In this case the 
      signature for the hook function is:
         void hook(void (*addr)());

      The option '--entry_parm=none' specifies that the hook is called with 
      no parameters.  This is the default. In this case the signature for 
      the hook function is:
      	 void hook(void);

Exit hooks are specified similarly using the --exit_hook and --exit_parm
options. The default function name is __exit_hook.

The presence of the hook options creates an implicit declaration of the hook 
function with the given signature. If a declaration or definition of the
hook function appears in the compilation unit compiled with the options,
it must agree with the signatures listed above.

In C++ the hooks are declared extern "C". This means you can define them in 
C (or assembly) without being concerned with name mangling.

Hooks may be declared inline, in which case the compiler will try to inline
them using the same criteria as other inline functions.

Entry hooks and exit hooks are independent. That is, you can enable one, the
other, or both. The same function can be used as both the entry and exit hook.

You must take care to avoid recursive calls to hook functions. This means
that the hook function should not call any function which itself has
hook calls inserted.  To help prevent this, hooks are not generated for
inline functions, or for the hook functions themselves. 

There is a pragma NO_HOOKS that can be applied to a function to prevent
hook calls from being generated for that function. 
   in C  : #pragma NO_HOOKS(func)    /* no hook calls from func() */
   in C++: #pragma NO_HOOKS()        /* no hook calls from next function */

The option --remove_hooks_when_inlining will remove entry/exit hooks for 
functions that are inlined by the optimizer.  

-------------------------------------------------------------------------------
6) Improved Support for the "restrict" Qualifier
-------------------------------------------------------------------------------

This release of the compiler has improved support for the restrict qualifier.
The restrict keyword is a type qualifier that may be applied to pointers, 
references, and arrays.  Its use represents a guarantee by the programmer 
that within the scope of the pointer declaration the object pointed to can 
be accessed only by that pointer.  Please see section 7.4.5 in the 
TMS320C6000 Optimizing Compiler User's Guide (spru187n) and the C99 standard
(ISO9899) for more information on the restrict keyword.  Also see section
3.8.2 in the TMS320C6000 Optimizing Compiler User's Guide (spru187n) for
information on the -mt compiler option.  The -mt option is an alternative to 
using the restrict keyword, which may be useful in cases where changes to the 
source are not possible.

In previous releases of the C6000 code generation tools, the compiler would
ignore the restrict keyword unless it was applied to a function parameter or 
pointer that was not a member of a structure or class object.  The pointer also
had to have file scope or function scope.  The 6.1.x C6000 compiler now has
better support for exploiting the restrict keyword.

The compiler will now utilize the restrict keyword when it is applied to:
  - all pointer variables, including globals, function parameters, and 
    variables with any scope, including block scope,
  - by-reference function parameters,
  - arrays of pointers or structs, and
  - class or structure pointer members.

In addition, the 6.1.x compiler is now better able to track and exploit
restrict-qualified function parameters (e.g. handling the case in which a
restrict-qualified parameter is reassigned in the function).  The compiler is
also better able to track and exploit a restrict-qualified function parameter
whose function is inlined.

-------------------------------------------------------------------------------
7) Support for the c674x ISA
-------------------------------------------------------------------------------

Capabilities:
1. Full support for c674x via option -mv6740
2. Includes the following c674x RTS variants:   
   rts6740.lib  rts6740_eh.lib  rts6740e.lib  rts6740e_eh.lib

Notes:
1. The c674x compiler sets the following predefines:
   _TMS320C6740
   _TMS320C6400, _TMS320C6400_PLUS 
   _TMS320C6700, _TMS320C6700_PLUS 
2. The c674x  assembler sets the following predefined symbols to 1:
   .TMS320C6740 
   .TMS320C6400, .TMS320C6400_PLUS
   .TMS320C6700, .TMS320C6700_PLUS
3. When assembling for c674x, asm6x follows the documented  
   encoding style for unary DP instructions i.e. it encodes 
   src1 as 0's

-------------------------------------------------------------------------------
8) Conditional linking changes to compiled C/C++ code
-------------------------------------------------------------------------------

The compiler now emits .clink directives for all code sections.

The compiler will now generate a .clink directive for all compiled functions.
The .clink directive enables conditional linking for that function when 
the -mo/--gen_func_subsection option is used.  This tells the linker to 
leave the function out of the final output of the linker if there are no 
references found to the function.  When -mo/--gen_func_subsections is NOT 
used, conditional linking will be performed on a file-level basis.  This 
means that the file will not be included in the final output of the linker 
if there are no references to any symbol defined in the file.  See document 
spru186 for more information on the .clink directive.

-------------------------------------------------------------------------------
9) The new FUNC_ALWAYS_INLINE pragma
-------------------------------------------------------------------------------

The compiler now has support for the FUNC_ALWAYS_INLINE pragma.  This pragma
tells the compiler to always inline calls to the named function if it is legal
to do so and the compiler is invoked with some level of optimization
(e.g. -O0).

The syntax of the pragma in C is: #pragma FUNC_ALWAYS_INLINE ( func );
The syntax of the pragma in C++ is: #pragma FUNC_ALWAYS_INLINE;

This pragma overrides the compiler's inlining decisions.  Overuse of the
pragma could result in increased compilation times or memory usage,
potentially enough to cause errors in the toolset.

-------------------------------------------------------------------------------
10) Call-site-based inlining 
-------------------------------------------------------------------------------

The compiler now more aggressively performs automatic inlining at -O3 when 
compiling for performance.

Automatic Inline Expansion (-oi Option)
---------------------------------------
When optimizing with the -O3 or -O2 option, the compiler automatically inlines
certain functions.

A command-line option, -oi<size>, specifies the size threshold for
auto-inlining.  This option controls only the inlining of functions that 
are not explicitly declared as inline.

When the -oi<size> option is not used, the compiler sets the size limit based
on the optimization level and the optimization goal (performance versus code
size).  If the -oi size parameter is set to 0, automatic inline expansion 
is disabled.  If the -oi size parameter is set to a non-zero integer, the 
compiler will automatically inline any function smaller than <size>.
(Note that this is a change from previous releases, which would inline
functions for which the product of the function size and the number of
calls to it was less than <size>.  The new scheme is simpler, but will
usually lead to more inlining for a given value of <size>.)

The compiler measures the size of a function in arbitrary units; however the
optimizer information file (created with the -on1 or -on2 option) reports 
the size of each function in the same units that the -oi option uses.
When -oi<size> is used, the compiler does not attempt to prevent inlining
that causes excessive growth in compile time or size;  use with care.

When -oi<size> option is not used, the decision to inline a function at a
particular call-site is based on an algorithm that attempts to optimize
benefit and cost.  The compiler inlines eligible functions at call 
sites until a limit on size or compilation time is reached.

When deciding what to inline, the compiler collects all eligible call-sites in
the module being compiled and sorts them by the estimated benefit over cost.
Functions declared static inline will be ordered first, then leaf functions,
then all others eligible.  Functions that are too big are not included.

Inlining behavior varies, depending on which compile-time options specified.
  - The code size limit is smaller when compiling for code size rather than
    performance.  -oi<size> overrides this size limit.
  - At -O3, the compiler will auto-inline aggressively if compiling for
    performance.
  - At -O2, the compiler will only automatically inline small functions.

Some Functions Cannot be Inlined
--------------------------------
For a call-site to be considered for inlining, it must be legal to inline the
function and inlining must not be disabled in some way.  See the inlining
restrictions below.

-O3 and -O2 Optimization and Inlining 
-------------------------------------
In order to turn on automatic inlining, you must use the -O3 or -O2 option.
At -O2, only small functions are auto-inlined.  If you desire the -O3 or
-O2 optimizations, but not automatic inlining, use -oi0 with the -O3 or -O2 
option.

Inlining and Code Size
----------------------
Inlining increases code size, especially inlining a function that is called in
several places.  Function inlining is optimal for functions that are called
only from a small number of places and for small functions.  To prevent
increases in code size because of inlining, use the -oi0 and -pi options;
these options, used together, cause the compiler to inline intrinsics only.

Inlining Restrictions
---------------------
There are several restrictions on what functions can be inlined for both
automatic inlining and definition-controlled inlining.

At a given call site, a function may be disqualified from inlining if it:
 - Is not defined in the current compilation unit
 - Never returns
 - Is recursive
 - Has a FUNC_CANNOT_INLINE pragma
 - Has a variable number of arguments
 - Has a different number of arguments than the call site
 - Has an argument whose type is incompatible with the corresponding call
   site argument
 - Has a structure argument
 - Contains a volatile local variable or argument
 - Is not declared inline and contains an asm() statement that is not a
   comment
 - Is not declared inline and it is main()
 - Is not declared inline and it is an interrupt function
 - Is not declared inline, returns void but its return value is needed
 - Is not declared inline and will require too much stack space for
   array or structure local variables

-------------------------------------------------------------------------------
11) New COFF Linker
-------------------------------------------------------------------------------

See the LINKER_README.txt file.
