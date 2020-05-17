TIMER CSL EXAMPLE
================
The folders in this directory contain a register layer example that demonstrates the usage of TIMER.

Example Description:
--------------------
This example initializes the timers 0 and 1 using CSL macros. They are configured
to run in one shot mode and generate interrupts upon overflow. The interrupt
occurance are verifed in the task and result is printed in the console.

Directory structure:
---------------------

src - contains the example source files
build  - contains the example project and the linker command file
obj\debug - contains the project output executble

To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the Timer_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Build the project.

6. Load the .out execuable file.

7. Execute.

