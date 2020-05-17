IDMA CSL EXAMPLE : Idma_example
================================
The folders in this directory contain a example that demonstrates the usage of IDMA CSL.

Example Description:
--------------------
This example initializes the IDMA peripheral for data transfer using the CSl macros.
The source and destination addresses are programmed using the CSL macros and data
at the end of transfer is verified.


Directory structure:
---------------------

src - contains the example source files
build  - contains the example project and the linker command file
obj\Debug - contains the example project output file


To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the Idma_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Include the libraries rts6740.lib

6. Build the project.

7. Load the .out executable file.

8. Execute.

