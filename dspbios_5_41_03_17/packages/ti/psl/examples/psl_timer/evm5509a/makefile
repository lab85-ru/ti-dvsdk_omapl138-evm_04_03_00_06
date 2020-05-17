#
#  Makefile for creation of the program named by the PROG variable
#
#  The following naming conventions are used by this makefile:
#       psl_timer.asm      - C55 assembly language source file
#       psl_timer.obj      - C55 object file (compiled/assembled source)
#       psl_timer.out      - C55 executable (fully linked program)
#

COMMON_DIR = ../../common

include $(COMMON_DIR)/config.mak
include $(COMMON_DIR)/c55rules.mak

#  Flags - Specify platform specific compiler options here

CC55FLAGS = -c -ml -dCHIP_5509A
AS55FLAGS = -ml
LD55FLAGS = -l"csl5509ax.lib" -l"PSL_cfg_evm5509a.a55L" -l"PSL_c5509a.a55L" -m"psl_timer.map"

PROG	= psl_timer
OBJS	= vectors.obj timer.obj funcs.obj
CMDS	= $(COMMON_DIR)/lnkx.cmd

#
#  Targets:
#
all: $(PROG).out

$(PROG).out: $(OBJS) $(CMDS)

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) *.obj *.out *.lst *.map
