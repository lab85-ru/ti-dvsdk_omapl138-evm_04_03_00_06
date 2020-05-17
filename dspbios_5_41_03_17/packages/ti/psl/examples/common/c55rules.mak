#
#  This file contains the definitions and rules necessary to 
#  build an application for the C55 with GNU make.
#

#The cl55 acts as the compiler, assembler, and linker for C55x architectures.  
#The "-z" option in LD55 specifies that all options following the "-z" will be
#linker options.

CC55 = $(C55X_CODEGEN_ROOT)/bin/cl55
AS55 = $(C55X_CODEGEN_ROOT)/bin/cl55
LD55 = $(C55X_CODEGEN_ROOT)/bin/cl55 -z -x

#specify the C55x compiler options
CC55OPTS = -g -I. -I.. -I$(PSL_ROOT)/include \
    -I$(PSL_ROOT)/include/c5500 \
    -I$(CSL_ROOT)/include/c5500 \
    -I$(C55X_CODEGEN_ROOT)/include

#specify the 55x assembler options
AS55OPTS = -I. -I.. -I$(PSL_ROOT)/include \
    -I$(CSL_ROOT)/include/c5500

#specify the 55x linker options
LD55OPTS = -c -q -I.. -I$(COMMON_DIR) -I$(PSL_ROOT)/lib \
    -I$(CSL_ROOT)/lib/c5500 \
    -I$(C55X_CODEGEN_ROOT)/lib -lrts55x.lib

#  Pattern rules to build C55 executables

%.obj : ../%.cpp
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : %.cpp
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : ../%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : %.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : ../%.asm
	@ $(CHMOD) $@
	$(AS55) $(AS55OPTS) $(AS55FLAGS) $<

%.obj : %.s55
	@ $(CHMOD) $@
	$(AS55) $(AS55OPTS) $(AS55FLAGS) $<

%.out : %.obj
	@ $(CHMOD) $@
	@ $(CHMOD) $*.map
	$(LD55) $(LD55OPTS) -o $@ $^ $(LD55FLAGS)
