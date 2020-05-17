I2C CSL EXAMPLE :
=================
The folders in this directory contain a example that demonstrates the usage of I2C CSL

Example Description:
--------------------
This example uses initializes the I2C peripheral in loopback mode using CSL macros.
The data is transmitted and received via this loopnack mode using CSL macros.


Directory structure:
---------------------

src - contains the example source files
build  - contains the example project and the linker command file
obj\Debug - contains the example project output file

To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the I2c_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Include the libraries rts6740.lib

6. Build the project.

7. Load the .out executable file.

8. Execute.
