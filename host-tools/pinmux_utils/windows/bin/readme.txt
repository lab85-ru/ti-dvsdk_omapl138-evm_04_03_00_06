Here are some basic instructions to install and use Pin Setup for AM18xx 

- Unzip to any directory (referred to as "<install_dir>")

- Run c:\<install_dir>\bin\PinSetup.exe 

- This only contains the pins where there are decisions to be made on pinmuxing. 
  Some functional pins are not listed here (e.g. JTAG, USB, RTC, ...)

- Device buttons will give you some ideas on pins that drop as you change part numbers.

- If you click the different device radio buttons they will gray out all 
  the pins not available on the device and show you what is available.

- If you click the different Peripherals Buttons, related pins will be selected.

- You can play around with the Edit -> Find. For example, use it to check (i.e. select)
  pins and you can see how it handles collisions.

- Double clicking on a cell enables the pin (or disables it if it's enabled).

- You can copy the registers to the clipboard and paste on desired file.
  Or, you can save them to a .h file from File -> Save -> Header File.

- You can save your pin configurations from File -> Save -> Pin Selections
  Also, you can load your pin configurations from File -> Load -> Pin File.


In the case of a discrepancy between the device model used in this program
and the data manual, the information in the data manual shall take precedence.

Please let us know if you find problems or have questions. Questions can be 
posted to the AM17x, AM18x ARM9 Microprocessors Forum on http://e2e.ti.com.

