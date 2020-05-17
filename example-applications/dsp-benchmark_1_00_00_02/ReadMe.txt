#############################################################
DSP Benchmark demo Version 01.00.00.02

Release Notes
August 2010
Build ID: 01.00.00.02
License: TI BSD
#############################################################

The release note is divided into the following sections:
• Description
• Package Content
• Device Supported
• Dependencies

-------------
Description
-------------
DSP benchmark demo is a demonstration of perforance of the C674X DSP on the OMAPL138 platform.
This demo aims at showcasing the DSP as an accelerator for an ARM users to improve application performance.

-----------------------------
PACKAGE CONTENT
-----------------------------
This software package contains the following subdirectories

docs : Contains User guide and the Release Notes on the DSP benchmark demo

dsp_benchmark_app - Contains demo application that displays DSP only performance and 
                    the performance of the DSP when called from the ARM using C6Accel

ReadMe.txt : Top level ReadMe file

Rules.make: Toplevel Rules.make file  to set path to dependencies

Makefile : Top level Makefile to build the package 

---------------
Dependencies
---------------

* C6Accel
* Codec Engine
* DSP BIOS
* DSPLINK
* Linux Utils
* Framework compoenents 

---------------------------
Devices Supported
---------------------------
* OMAPL138

