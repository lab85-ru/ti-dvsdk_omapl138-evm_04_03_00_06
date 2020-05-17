GPIO CSL EXAMPLE
====================
The folders in this directory contain a example that demonstrates the usage of GPIO CSL.

Example Description:
--------------------
This example intiatilzes the GPIO peripheral using CSL macros, which are used to
configure two GPIO pins one each in input and output modes. The input pin (bank)
interrupt is enabled to receive interrupt from that pin when input transition
(high to low) occurs at that pin. The interrupt handler registered for this
interrupt then toggles the output pin in loop, which make the LED connected at this
pin to blink.

Directory structure:
---------------------

src - contains the example source files
build  - contains the example project and the linker command file
obj\Debug - contains the example project output file

To Run the Example in the little endian mode:
-------------------------------------------

1. Choose the Little endian configuration in the CCS setup

2. Open the Gpio_example.pjt in the CCS

3. Ensure that the gel file is loaded.

4. In the project build options configure the endianness to little endian

5. Build the project.

6. Load the .out executable file.

7. Execute.(Please refer to the verbatim section of the Gpio_example file for
   the procedure)


