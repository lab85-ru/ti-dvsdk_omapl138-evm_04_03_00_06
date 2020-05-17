# This make file is used by each example's makefile.  The purpose of this 
# file is to define all global variables, used by each makefile, in one place
# to eliminate having to edit multiple files for the same desired change.

#define the full path of the install directory in which PSL was installed:
INSTALL_ROOT = ../../../ti

#define the full path of the root directory in which your CSL lives:
COMPONENT_ROOT = ../../../../imports

#change these variable to point to your location.  
PSL_ROOT = $(INSTALL_ROOT)/psl
CSL_ROOT = $(COMPONENT_ROOT)/ti/csl
TCONF_ROOT = #define the full path to your TCONF installation

#define location of code generation tools here
C55X_CODEGEN_ROOT = #define full path to 55x codegen tools here

#uncomment this REMOVE definition to build on Windows:
#REMOVE = $(TCONF_ROOT)/bin/rm.exe -f

#use this definition of REMOVE for Solaris and Linux
REMOVE = rm -f

#uncomment this CHMOD definition to build on Windows:
#CHMOD = attrib -r > NUL

#use this definition of CHMOD for Solaris and Linux:
CHMOD = chmod -f +w
