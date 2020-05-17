CSL 2.0 DAT Adapter Reference Implementation Using EDMA3 Low Level Driver

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Following directories are part of this package:-

CSL2_DAT_DEMO             - Top level directory
    - csl2_legacy_include     - Legacy headers of CSL 2.0
    - dat_edma3LLD            - Reference implementation of the DAT APIs using 
                                the EDMA3 Low level driver package
    - demo                    - Example pjt to run on DM6446 EVM, 
                                that makes calls DAT APIs
    - bios_adapter            - BIOS specific implementation of critical section                                entry exit APIs required by EDMA3 LLD as well as
                                the Dat wrapper
                              - Interrupt registration and un-registration code
                                required by the application
    - docs                    - CSL_DAT_Adapter presentation 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Following environment variables need to be set to correctly build the pjt
EDMA3LLD_INSTALL_DIR     Location of EDMA3 Low level driver package
                         installation 
                         (NOTE:- Ensure EDMA3 low level driver package is
                                 built specifically for DM6446)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Issues:-
1. Due to a DM6446 Simulation bug, this example does not run on the simulator 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
