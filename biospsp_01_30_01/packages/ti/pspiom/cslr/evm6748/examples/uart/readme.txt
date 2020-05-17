UART CSL EXAMPLE
====================
The folders in this directory contain a register layer example for UART. This
example could also be seen as an example for PSC CSL layer API usage, since this
example also uses the PSC CSL APIs to bring UART module out of sleep mode.

Example description:
--------------------
This example initializes the UART in loopback mode using CSL macros. This
initialization is verified by comparing the transmit and receive data.

Directory structure:
---------------------

src - contains the example source files
build  - contains the example projects and the linker command file
obj\debug - contains the project output executable


To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the Uart_example.pjt in the CCS

3. Ensure that the gel file is loaded

4. In the project build options configure the endianness to little endian

5. Ensure that the libraries rts6740.lib is included.

6. Build the project.

7. Load the .out executable file.

8. Execute.




