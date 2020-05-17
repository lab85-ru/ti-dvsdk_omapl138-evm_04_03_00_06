#
#  This file contains the definitions and rules necessary to 
#  build configuration library for the C55 with GNU make.
#

#The cl55 acts as the compiler, assembler, and linker for C55x architectures.  
#The "-z" option in LD55 specifies that all options following the "-z" will be
#linker options.
CC55 = $(C55X_CODEGEN_ROOT)/bin/cl55
AS55 = $(C55X_CODEGEN_ROOT)/bin/cl55
LD55 = $(C55X_CODEGEN_ROOT)/bin/cl55 -z -x
AR55 = $(C55X_CODEGEN_ROOT)/bin/ar55

#specify the C55x compiler options
CC55OPTS = -o2 -I. -I.. -I$(PSL_ROOT)/include \
    -I$(PSL_ROOT)/src/include \
    -I$(PSL_ROOT)/include/c5500 \
    -I$(CSL_ROOT)/include/c5500 \
    -I$(C55X_CODEGEN_ROOT)/include

#specify the 55x assembler options
AS55OPTS = -I. -I.. -I$(PSL_ROOT)/include \
    -I$(CSL_ROOT)/include/c5500

#specify the 55x linker options
LD55OPTS = -c -q -I.. -I$(COMMON_DIR) -I$(PSL_ROOT)/lib \
    -I$(CSL_ROOT)/lib/c5500 \
    -I$(C55X_CODEGEN_ROOT)/lib

#  Pattern rules to build C55 libraries

%.obj : $(VOLT_REG)/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(VOLT_REG_NULL)/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(VOLT_REG_GPIO)/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(VOLT_REG_GPIO)/%.s55
	@ $(CHMOD) $@
	$(AS55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/c5503/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/c5507/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/evm5509a/%.s55
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/evm5509a/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/dsk5510/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(VOLT_REG)/%.s55
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/c5510/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.obj : $(PSL_CFG)/c5509a/%.c
	@ $(CHMOD) $@
	$(CC55) $(CC55OPTS) $(CC55FLAGS) $<

%.a55L : $(LIB)
	@ $(CHMOD) $@
	$(AR55) -a $@ $(OBJS)
	@ echo cleaning up ...
	@$(REMOVE) *.obj


