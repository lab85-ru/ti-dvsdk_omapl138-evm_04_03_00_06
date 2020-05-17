#
#  Makefile for creation of the program named by the PROG variable
#
#  The following naming conventions are used by this makefile:
#       psl_basic.asm      - C55 assembly language source file
#       psl_basic.obj      - C55 object file (compiled/assembled source)
#       psl_basic.out      - C55 executable (fully linked program)
#

COMMON_DIR = ../../common

include $(COMMON_DIR)/config.mak
include $(COMMON_DIR)/c55rules.mak

#  Flags - Specify platform specific compiler options here

CC55FLAGS = -c -ml -v5510:2 -dCHIP_5510PG2_2
AS55FLAGS = -ml
LD55FLAGS = -l"PSL_cfg_dsk5510.a55L" -l"PSL_c5510.a55L" -m"psl_basic.map"

PROG	= psl_basic
OBJS	= funcs.obj
CMDS	= $(COMMON_DIR)/lnkx.cmd

#
#  Targets:
#
all: $(PROG).out

$(PROG).out: $(OBJS) $(CMDS)

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) *.obj *.out *.lst *.map
