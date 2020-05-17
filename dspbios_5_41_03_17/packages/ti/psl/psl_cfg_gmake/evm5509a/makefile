#
#  Makefile for creation of the evm5509a configuration library
#

COMMON_DIR    = ../common
VOLT_REG_NULL = ../../volt_reg/null
VOLT_REG_GPIO = ../../volt_reg/gpio55x
PSL_CFG       = ../../psl_cfg
VOLT_REG      = ../../volt_reg/evm5509a

include $(COMMON_DIR)/config.mak
include $(COMMON_DIR)/c55rules.mak

#  Flags - Specify platform specific compiler options here

CC55FLAGS = -c -ml -d_5509_
AS55FLAGS = -ml

LIB	= PSL_cfg_evm5509a
OBJS	= PSLvolt_cfg_evm5509a.obj PSLclk_cfg_evm5509a.obj PSLvolt_reg.obj \
          PSLvolt_reg_delay.obj PSLvolt_reg_evm5509a.obj
CMDS	= 

#
#  Targets:
#
all: $(LIB).a55L

$(LIB).a55L: $(OBJS) $(CMDS)

.clean clean::
	@ echo removing object files and binaries ...
	@$(REMOVE) *.obj *.a55L *.lst *.map *.out
