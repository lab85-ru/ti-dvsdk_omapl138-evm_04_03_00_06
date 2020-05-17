#
#  Makefile for creation of the dsk5510 configuration library
#

COMMON_DIR    = ../common
VOLT_REG_NULL = ../../volt_reg/null
VOLT_REG_GPIO = ../../volt_reg/gpio55x
PSL_CFG       = ../../psl_cfg
VOLT_REG      = ../../volt_reg/dsk5510

include $(COMMON_DIR)/config.mak
include $(COMMON_DIR)/c55rules.mak

#  Flags - Specify platform specific compiler options here

CC55FLAGS = -c -ml -d_5510_
AS55FLAGS = -ml

LIB	= PSL_cfg_dsk5510
OBJS	= PSLvolt_cfg_DSK5510.obj PSLclk_cfg_DSK5510.obj \
          PSLvolt_reg.obj PSLvolt_reg_delay.obj \
	  PSLvolt_reg_DSK5510.obj PSLcpld_5510DSK.obj
CMDS	= 

#
#  Targets:
#
all: $(LIB).a55L

$(LIB).a55L: $(OBJS) $(CMDS)

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) *.obj *.a55L *.lst *.map *.out
