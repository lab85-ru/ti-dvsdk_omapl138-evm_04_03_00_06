ReadMe

C6Accel Version 01.01.00.07

Release Notes
October 2011
Build ID: 01.01.00.07

The release note is divided into the following sections:
• Description
• Package Contents
• Host Support
• Device Support
• Dependencies
• Validation information
• Issues fixed

-------------
Description
-------------
C6Accel is an xdais algorithm created to facilitate pre and post processing of data on DSP in an SOC environment.
This codec provides canned DSP software to ARM user to improve performance by leveraging DSP as an accelerator.

-----------------------------
PACKAGE CONTENT
-----------------------------
This software package contains the following subdirectories

docs : Contains documentation of C6Accel Installation & Usage guide, Reference API guide and Advance user guide
         and benchmarking information

dsp - Contains common DSP-side components of C6Accel
       *alg : Contains xdais algorithm of C6Accel using the iUniversal codec engine interface
       *libs: Contains DSP software libraries that are currently linked to C6Accel

soc- Contains hetrogeneous ARM+DSP code 
      * app : Contains sample test application that tests and benchmarks kernels in C6Accel
      * c6accelw: Contains C6Accel wrapper API library which is aimed at simplifying user experience 
      * packages: Contains C6Accel codec package and unit server on OMAPL and OMAP3 platform

-------------------------
Package Dependecies 
-------------------------
Note: Version numbers are specific to devices. Check validation information for details
DSP/BIOS 

XDCTools 

C6000 Code Generation Tools (CGT) 
  
DSPLink 

Linuxutils 
 
Codec Engine v2.21 and higher
 
Code Sourcery G++ Lite 2009q1-203 toolchain for ARM 

Xdais 

Ceutils 

EDMA LLD 

-----------------------------
HOST SUPPORT
-----------------------------
This release supports installation and development on Linux as well as Windows workstation.

-----------------------------
DEVICE SUPPORT
-----------------------------
This release supports the Texas Instruments DM3730, OMAP3530, OMAPL138 EVM boards.

--------------------------              
New Validation Information
--------------------------
 Platform Name                          Dependencies and their versions used in testing
*  OMAP3530/DM3730                      Linux 2.6.37, Codec Engine 2.25.2.11, TI CGT 6.1.14, xdctools 3.16.03.36, linux utils 2.26.02.08, dsplink 1.65.00.02,bios  5.41.03.17, framework components 2.25.01.05, xdais 6.25.02.11, bios utils 1.02.02)

*  OMAPL138                             Linux 2.6.37, Codec Engine 2.25.2.11, TI CGT 6.1.14, xdctools 3.16, linux utils 2.26, dsplink 1.65,bios  5.41, framework components 2.25.01.05, xdais 6.25.02.11, bios utils 1.02.02

---------------------------
Updates:
---------------------------
* Added 60 VLIB APIs to C6Accel and 5 IMGLIB APIs.
* Modified the DM3730 Memory map to align with the SDK memory map.
* Aligned with linux kernel 2.6.37 and associated linux modules (LPM, Linuxutils).

-------------------------------------------------------------------------------------------------------
Documentation:
-------------------------------------------------------------------------------------------------------
The most recent documentation can be viewed on http://processors.wiki.ti.com/index.php/C6Accel:_ARM_access_to_DSP_software_on_TI_SoCs

The advantage of looking at Wiki:
•	Easy readability
•	There may be corrections of modifications for improving the documentation

Note: The documentation corresponds to the latest release version of C6Accel [release 1.01.00.07] so may not be in sync with the older releases. 
