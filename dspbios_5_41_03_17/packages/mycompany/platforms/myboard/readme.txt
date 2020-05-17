If you need to create your own platform, follow these steps:
1. In the right column "Devices", find the device used on your board, and then
   determine the name of the configuration template that corresponds to that
   device from the column "Platforms". 

2. In this directory find the TCI file with the same name as the selected
   configuration template.

3. Open the file and edit the mem_ext, device_regs, and params structures to
   match the external memory, register, and CPU settings for your board.

4. The modified template file now contains the description of your board. You
   can now rename the template file to Platform.tci and use your platform as
   a starting point for your application configurations. 
   For more information, refer to Chapter "Tconf Platform Files" and Appendix B
   of the DSP/BIOS Tconf User's Guide (SPRU007), located in the xdctools/doc
   directory. Additional help is available in the CCS Help under
   DSP/BIOS 5.30 Help->Tconf->Create a New Platform. 



Platform template			Devices
-------------------	--------------------------------------------------------

Platform_28_Group1	2801, 2806, 2808, 2810, 2811, 2812

Platform_54_Group1	5401, 5402, 5402A, 5404, 5405, 5407, 5409, 5409A, 5410,
			5410A, 5416, 5420, 5470, 5471, 54CST, DM270, DM310,
			DM320

Platform_55_Group1	5501, 5502

Platform_55_Group2	5503, 5507, 5509, 5509A, 5510, 5510A, DA255

Platform_55_Group3	1510, 1610, 1710, 5903,	5905, 5910, 5912, 5944, 5946,
			5948, 2420, TNETV1050, TNETV1055 

Platform_55_Group4	DA295, DA300, 5561

Platform_55P_Group1	RYUJIN

Platform_62_Group1	6201, 6202, 6203, 6203B, 6204, 6205, 6701

Platform_67_Group1	6211, 6211B, 6711, 6711B, 6712, 6713

Platform_67P_Group1	6722, 6726, 6727, DA700, DA705, DA707, DA710, DA700SIM


Platform_64_Group1	6410, 6411, 6412, 6413, 6414, 6415, 6416, 6418, DM640,
			DM641, DM642, DRI300

Platform_64P_Group1	DM415, DM420, DM421, DM425, DM426, DM6443, DM6446,
			TCI6482, 6455, 2430, 3430

