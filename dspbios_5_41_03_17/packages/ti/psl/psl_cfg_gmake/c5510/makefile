#
#  Makefile for creation of the c5510 configuration library
#

COMMON_DIR    = ../common
VOLT_REG_NULL = ../../volt_reg/null
VOLT_REG_GPIO = ../../volt_reg/gpio55x
PSL_CFG       = ../../psl_cfg

include $(COMMON_DIR)/config.mak
include $(COMMON_DIR)/c55rules.mak

#  Flags - Specify platform specific compiler options here

CC55FLAGS = -c -ml -d_5510_ -D_PSL_NULL_VOLT_REG_
AS55FLAGS = -ml

LIB	= PSL_cfg_c5510
OBJS	= PSLvolt_cfg_null.obj PSLvolt_reg_null.obj \
          PSLclk_cfg_c5510.obj
CMDS	= 

#
#  Targets:
#
all: $(LIB).a55L

$(LIB).a55L: $(OBJS) $(CMDS)

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) *.obj *.a55L *.lst *.map *.out
