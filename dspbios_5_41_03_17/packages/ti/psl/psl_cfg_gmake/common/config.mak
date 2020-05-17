# This make file is used by each test's makefile.  The purpose of this 
# file is to define all global variables, used by each makefile, in one place
# to eliminate having to edit multiple files for the same desired change.

#define the full path of the install directory in which PSL was installed:
INSTALL_ROOT = ../../../../../ti

#define the full path of the root directory in which your CSL lives:
COMPONENT_ROOT = ../../../../../../imports

#change these variable to point to your location.  
PSL_ROOT = $(INSTALL_ROOT)/psl
CSL_ROOT = $(COMPONENT_ROOT)/ti/csl

#define location of code generation tools here
C6X_CODEGEN_ROOT = /db/toolsrc/library/vendors/c6x/4.32/Solaris
C55X_CODEGEN_ROOT = /db/toolsrc/library/vendors/c55/lead3v2.56/Solaris

#uncomment this REMOVE definition to build on Windows:
#REMOVE = $(INSTALL_ROOT)/tconf/bin/rm.exe -f

#use this definition of REMOVE for Solaris and Linux
REMOVE = rm -f

#uncomment this CHMOD definition to build on Windows:
#CHMOD = attrib -r > NUL

#use this definition of CHMOD for Solaris and Linux:
CHMOD = chmod -f +w
