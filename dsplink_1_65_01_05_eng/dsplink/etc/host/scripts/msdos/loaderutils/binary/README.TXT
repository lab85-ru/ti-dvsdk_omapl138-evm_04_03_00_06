#   ============================================================================
#   @file   README.TXT
#
#   @path   $(DSPLINK)/etc/host/scripts/msdos/loaderutils/binary/
#
#   @desc   Contains instructions for usage of loader for DSP executable in
#           binary format.
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


Description:
~~~~~~~~~~~~
The binary loader provided with DSP/BIOS LINK allows loading of DSP executable
files in the raw binary format.

The binary loader can be used only with the PrOS OS running on the GPP.
The below instructions are for using binary loader with Pseudo file system. For
using PrFILE file system, additional changes would be required in the
application to open the binary file and load its contents into a local array,
passing its address to the PROC_Load () call.

--------------------------------------------------------------------------------

Sequence of steps for using the binary loader:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(Refer to the details for these steps given in later sections.)
1.  Build the GPP-side of DSP/BIOS LINK.
2.  Choose the binary loader within the dynamic configuration for DSP/BIOS LINK.
3.  Build the DSP-side of DSP/BIOS LINK.
4.  Build the DSP-side sample application.
5.  Confirm that the paths to tools in binloader.pl script are correct.
6.  Run the binloader.pl script to generate required files.
7.  Make the required changes to the GPP-side sample application to use the
    binary loader.
8.  Build the GPP-side sample application with the new dynamic configuration.
9.  Build the pseudo file system with the DSP binary executable using the
    pseudofs tool to generate the FILESYS.LIB library. Include the FILESYS.LIB
    library in the build for PrOS GPP-side executable.
10. Build the GPP-side PrOS executable.

--------------------------------------------------------------------------------

Choosing the binary loader within the dynamic configuration for DSP/BIOS LINK:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1. To use the binary loader, the appropriate option for the DSP executable
   loader type must be selected within the dynamic configuration.

2. When the binary loader is to be used within DSP/BIOS LINK, the DSP object
   configuration within the application's copy of its platform-specific dynamic
   configuration file (copy of CFG_<PLATFORM>.c) must be modified as follows:
   Within the LINKCFG_dspObjects array definition for DSP objects:

   "BINARY",              /* LOADERNAME     : Name of the DSP executable loader */

   This modification plugs the binary loader into the DSPLINK loader such that
   the PROC_Load () call eventually passes down into the binary loader
   functions.

After this modification, the application's copy of the dynamic configuration
file must be compiled and linked along with the application.

--------------------------------------------------------------------------------

Changes required to applications using the binary loader:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Some changes are required to applications to be able to use the binary loader.
The binary loader expects information about the binary file to be loaded,
through the imagePath parameter to the PROC_Load () API:
EXPORT_API
DSP_STATUS
PROC_Load (IN   ProcessorId  procId,
           IN   Char8 *      imagePath,
           IN   Uint32       argc,
           IN   Char8 **     argv) ;

In the normal COFF loader, the imagePath parameter is the path (including file
name) of the DSP executable in the GPP file system.
In case of the binary loader, the imagePath parameter is a pointer to a specific
structure BINLOADER_ImageInfo (defined within loaderdefs.h) expected by the
binary loader.
typedef struct BINLOADER_ImageInfo_tag {
    Uint32  gppAddr     ;
    Uint32  dspLoadAddr ;
    Uint32  dspRunAddr  ;
    Uint32  fileSize    ;
    Uint32  shmBaseAddr ;
} BINLOADER_ImageInfo ;

The application is expected to set the fields within this structure providing
information about the DSP binary file to the loader. The address of this
structure is then provided to the PROC_Load () API as the imagePath parameter.

--------------------------------------------------------------------------------

Using the binloader.pl utility with the binary loader:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The binloader.pl utility is provided to simplify usage of the binary loader with
user applications.
The binloader utility generates source and header files with information about
the DSP executable to be loaded by the binary loader. It also converts the DSP
executable from COFF into binary format, which can be loaded by the binary
loader.

Usage instructions:
  Usage: binloader.pl <GPP o/p path> <DSP i/p file>
       <GPP o/p path>: Path where GPP-side files should be generated.
       <DSP i/p file>: Name (with path) of input DSP COFF executable.

  Example: binloader.pl L:\dsplink\gpp\src\samples\message\PrOS
           L:\dsplink\dsp\export\BIN\DspBios\Davinci\RELEASE\message.out

  (Note: Enter the above instruction in single line)

The files generated are:
1. 'C' file containing an instance of the BINLOADER_ImageInfo
   structure required by the binary loader.
   This generated file can be built with the ARM-side example to
   generate information used by the binary loader.
2. 'H' file containing declaration of the instance of the
   BINLOADER_ImageInfo structure in the 'C' file. This file can be
   included by the ARM-side application using the binary loader.
3. 'BIN' file generated through COFF to binary conversion of the
   DSP executable.
4. 'DAT' file, which is an intermediate file generated by the script
   and used for conversion of the DSP executable from COFF to
   binary format. This file is available to the user for reference
   about the memory map used for generation of the binary file.

The binary file is generated using the hex6x utility provided with the CGTOOLS
for C6000. The binary file is generated with optimum memory requirements for
loadable sections as shown in the DAT file also generated by the tool.


Changes required to applications when using the binloader.pl utility:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Some changes are required to applications to be able to use the binary loader
with files generated by the binloader.pl utility. Modifications to the message
   sample used with PrOS are demonstrated here as an example.
1. The generated DSP binary file is placed in the GPP file system. If using the
   pseudo file system, the pseudofs tool is used to generate a pseudo file
   system with the binary DSP executable to be loaded using the binary loader.
   To generate the pseudo file system using the pseudofs tool:
   cd $(DSPLINK)\etc\host\tools\generic\pseudofs
   gmake filesys
   gmake -s [debug/release]

   This generates the FILESYS.LIB library within the following directory:
   $(DSPLINK)\gpp\export\BIN\PrOS\Davinci\<DEBUG/RELEASE>

   This library must be included in the link step while generating the GPP-side
   executable.

2. The generated 'C' file is built along with the GPP application.
   Modify the SOURCES file in $(DSPLINK)\gpp\src\samples\message\PrOS to include
   the message_binloader.c file in the build:
   SOURCES :=  message_os.c         \
               message_binloader.c

3. The GPP application code is modified as follows. Note that when
   the binary loader is used, arguments cannot be passed to the DSP executable
   main () function through the PROC_Load () API parameters:

- Include loaderdefs.h in the application, which contains the definition of
  the BINLOADER_ImageInfo structure:
#include <loaderdefs.h>

- Include the generated 'H' file in the application:
#include <message_binloader.h>


- Have an extern declaration of the array created through the pseudofs tool,
  which corresponds to the binary DSP executable to be loaded.
/** ============================================================================
 *  @name   data_0000
 *
 *  @desc   Declaration of array corresponding to the binary file to be loaded,
 *          as generated through the pseudofs tool.
 *  ============================================================================
 */
extern CONST Uint8 data_0000 [] ;

- When loading the binary file through PROC_Load () API, access the structure
  containing information about the binary file as generated by the binloader
  utility and set the gppAddr field within this structure to the data_0000
  array. Pass the address to this structure to the PROC_Load API as its
  imagePath parameter.

NORMAL_API
DSP_STATUS
MESSAGE_Create (IN Char8 * dspExecutable, IN Char8 * strNumIterations)
{
    BINLOADER_ImageInfo *    imageInfo ;
    ...

    imageInfo = (BINLOADER_ImageInfo *) MESSAGE_DspBinLoaderInfo ;

    /* Use the array generated by the pseudofs file system. */
    imageInfo->gppAddr = (Uint32) data_0000 ;

    /* args cannot be sent to DSP exec when binary loader is used. */
    status = PROC_Load (ID_PROCESSOR, (Char8 *) imageInfo, 0, NULL) ;
    ...
}

--------------------------------------------------------------------------------
