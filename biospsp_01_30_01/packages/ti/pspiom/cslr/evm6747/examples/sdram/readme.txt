EMIFB/SDRAM CSL EXAMPLE
====================
The folders in this directory contain a register layer example for EMIFB/SDRAM

Example Description:
--------------------
This example initialzes the EMIFB peripheral using the CSL macros. The settings are
then verifed by writing and reading to the SDRAM connected to the EMIFB lines.
The read and writted data are compared and a result is output on the console.

Directory structure:
---------------------

src - contains the example source files
build  - contains the example projects and the linker command file
obj\debug - contains the project output executable file

To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the sdram_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Ensure that the libraries rts6740.lib

6. Build the project.

7. Load the .out executable file.

8. Execute.




