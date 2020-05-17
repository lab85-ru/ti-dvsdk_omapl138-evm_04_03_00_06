PLLC CSL EXAMPLE
====================
The folders in this directory contain a register layer example for PLLC

Example Description:
--------------------
This example initialzes the DA83X PLL controller using CSL macros.

Directory structure:
---------------------

src - contains the example source files
build  - contains the example projects and the linker command file
obj\debug - contains the project output executable file

To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the Pllc_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Ensure that the libraries rts6740.lib

6. Build the project.

7. Load the .out executable file.

8. Execute.




