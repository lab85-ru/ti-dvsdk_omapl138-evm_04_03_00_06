#############################################################
DSP Benchmark demo 
User Guide
August 2010
version 1.00.00.02
#############################################################
-------------------------------------------------------------
*************************************************************
Installing the Demo
*************************************************************
-------------------------------------------------------------

1. Download the setup file dsp_benchmark_demo-x.x-Linux-x86-Install from the target content page into a temperory folder (eg
   /tmp).
2. To install the DSP_benchmark_demo package using the Linux installer,log in using a user account . The user account must have
   execute permission for the all the installation files. Switch user to “root” on the host Linux workstation and change
   directories to the temporary location where you have downloaded the bin files. Once you have changed the
   execute permissions you can go back to a normal user.
   host    $  su root
   host    $  cd /tmp
   host    $  chmod +x *.bin
   host    $  exit
3. Execute the installer that you previously downloaded from the SDK target content download page.
  For example:
   host $ cd /tmp
   host $ ./dsp_benchmark_demo-x.x-Linux-x86-Install

Directory structure The C6Accel installation provides the directory structure below:
DSP_benchmark_demo_x_xx_xx_xx
|
+−−dsp_benchmark_app: Demo application code for benchmarking the DSP
|       
|
+−−docs: documentation for C6Accel
|
+−−README.txt Top−level README file

------------------------------------------------------------------
******************************************************************
Building the DSP Benchmark Demo and Running the Demo
******************************************************************
------------------------------------------------------------------
Assumption: The Build step assumes that the linux kernel, cmem and dsplink module and C6Accel has been pre-built. If they are
not built users are expected to build those dependencies before building the DSP_benchmark_demo package.
1. Set paths to dependencies in the Rules.make file in the package. Ensure the C6ACCEL_INSTALL_DIR and DEMO_INSTALL_DIIR is set to
   the path where the C6accel package is installed
      C6ACCEL_INSTALL_DIR = /home/user/dsp_benchmark_demo_x_xx_xx_xx
      DEMO_INSTALL_DIR = /home/user/dsp_benchmark_demo_x_xx_xx_xx
2. Build the package by using the command make command in the root directory of the package.
      make all
3. Set the EXEC_DIR in Rules. make Nd Install the test app in the work area on the filesystem of the target by executing
      make install
4. Power on the device, open Teraterm/Minicom to view the boot and the command propmt for the target. Change
directory to working area on the filesystem.
   cd $(EXEC_DIR)
5. Load the cmem and dsplink modules by running the shell script loadmodules_omapl138.sh
    ./loadmodules_c6accel_omap3530.sh
6. Run the app by executing the following command on the command prompt on the target.
    ./dsp_benchmark_app

----------------------------------------------------------------------
**********************************************************************
Demo Details
**********************************************************************
----------------------------------------------------------------------

Benchmarks are provided to highlight the performance capabilities of DSP
 Two simultaneous benchmark are provided: 
* DSP only benchmarks: Time taken for processing on the DSP [@300MHz]
* ARM side DSP benchmark: Time taken to invoke a DSP function from the ARM (using C6Accel)

The demo provides DSP benchamrks for functions in the following categories details of which are presented below
[+] Signal processing
    	> DSP_fft16x16 ( 16bit Fixed point FFT )
	> DSP_fft32x32 (32 bit fixed point FFT )
	> DSP_autocorrelation (Fixed point autocorrelation)
	> DSPF_fftSPxSP (Single Precision FFT)
	> DSPF_ifftSPxSP (Single Precision Inverse FFT)
	> DSPF_iir (FLoating point IIR Filtering)
        > DSPF_fir_gen (Floating point FIR Filtering)
	    
[+] Image Processing
        > IMG_histogram8 (8 bit Histogram)
	> IMG_conv_3x3_i8_c8s (8 bit 3x3 Mask Image Convolution)
	> IMG_sobel_3x3_8 (8 bit 3x3 Sobel Edge Detection)
	> IMG_adds_8 (8 bit Image Addition)
	> IMG_yc_demux_8 (8 bit Deinterlacing YUV422ILE to YUV422pl)
        > IMG_yuv422pl_to_rgb565 (8 bit YUV planar to RGB conversion)

[+] Math Processing
        > EXPSP/EXPDP (Single and double precision exponential function)
        > SINSP/SINDP (Single and double precision Sine function)
        > LOGSP/LOGDP (Single and double precision Log function)
        > COSSP/COSDP (Single and double precision Cosine function)
        > ATANSP/ATANDP (Single and double precision Arc Tangent function)
        > POWSP/POWDP (Single and double precision Power function)

For Implementation details of these kernels refer to correpsonding Library available on TI.com or C6Accel documentation
http://processors.wiki.ti.com/index.php/C6Accel:_ARM_access_to_DSP_software_on_TI_SoCs
 






       


