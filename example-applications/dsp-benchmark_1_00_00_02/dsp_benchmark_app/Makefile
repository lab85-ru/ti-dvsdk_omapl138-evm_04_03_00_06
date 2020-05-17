ROOTDIR = ../
TARGET = dsp_benchmark_app

include $(ROOTDIR)/Rules.make

# Comment this out if you want to see full compiler and linker output.
#VERBOSE = @

# Which compiler flags should be used?
BUILD_TYPE=release

# Where to output configuration files
XDC_CFG		= $(C6ACCEL_INSTALL_DIR)/soc/app/c6accel_app_config

# Output compiler options
XDC_CFLAGS	= $(XDC_CFG)/compiler.opt

# Output linker file
XDC_LFILE	=  $(XDC_CFG)/linker.cmd

# Input configuration file
XDC_CFGFILE	= $(PLATFORM)/$(TARGET).cfg

# Target tools
XDC_TARGET	= gnu.targets.arm.GCArmv5T

CROSS_COMPILE	= $(CSTOOL_PREFIX)

export CROSS_COMPILE
export CODEGEN_INSTALL_DIR
export XDCPATH
export PLATFORM_XDC

# The XDC configuration tool command line
CONFIGURO = $(XDC_INSTALL_DIR)/xs xdc.tools.configuro
CONFIG_BLD = $(C6ACCEL_INSTALL_DIR)/config.bld

ifeq ($(BUILD_TYPE), release)
ifeq ($(ARM_ISA),armv7-a)
    C_FLAGS	+= -O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp
endif
ifeq ($(ARM_ISA),armv5t)
    C_FLAGS	+= -mlittle-endian -march=armv5t -mtune=arm9tdmi -mabi=aapcs-linux -O
endif
else
    CPP_FLAGS	+= -DNDEBUG
    C_FLAGS	+= -Wall -g
endif

ifeq ($(PLATFORM),omapl138)
    C_FLAGS	+= -DPLATFORM=138
endif

LD_FLAGS += -L$(LINUXLIBS_INSTALL_DIR)/lib -lm -lpthread 
C6ACCEL_LIB += $(C6ACCEL_INSTALL_DIR)/soc/c6accelw/lib/c6accelw_$(PLATFORM).a470MV

COMPILE.c = $(VERBOSE) $(CSTOOL_PREFIX)gcc $(CPP_FLAGS) $(C_FLAGS) $(CPP_FLAGS) -c
LINK.c = $(VERBOSE) $(CSTOOL_PREFIX)gcc $(LD_FLAGS)

INCLUDES = -I$(C6ACCEL_INSTALL_DIR)/soc/c6accelw/ \
           -I$(XDC_CFG)
           
		
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJFILES = $(SOURCES:%.c=%.o)

.PHONY: clean install 

all:	$(TARGET)

install:	$(if $(wildcard $(TARGET)), install_$(TARGET))

install_$(TARGET):
	@echo Copying dsp benchmark app 
	@install $(TARGET) $(EXEC_DIR_DEMO)
	@cp -R test_files $(EXEC_DIR_DEMO)
	@echo
	@echo Installed $(TARGET) binaries to $(EXEC_DIR_DEMO)..
	@echo
	@echo Copying kernel modules to target file system: $(EXEC_DIR_DEMO)/
	@echo Copying cmemk.ko
	@install -m 755 $(CMEM_INSTALL_DIR)/packages/ti/sdo/linuxutils/cmem/src/module/cmemk.ko $(EXEC_DIR_DEMO)/
	@echo Copying dsplinkk.ko
	@install -m 755 $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL138/RELEASE/dsplinkk.ko $(EXEC_DIR_DEMO)/
	@echo Copying c6accel codec servers from $(C6ACCEL_INSTALL_DIR)
	@install -m 755 $(C6ACCEL_INSTALL_DIR)/soc/packages/ti/c6accel_unitservers/omapl138/c6accel_omapl138.x674 $(EXEC_DIR_DEMO)/
	@chmod -x $(EXEC_DIR_DEMO)/*.x674 $(EXEC_DIR_DEMO)/*.ko 
	@install -m 755 $(C6ACCEL_INSTALL_DIR)/soc/app/omapl138/loadmodules_omapl138_c6accel.sh $(EXEC_DIR_DEMO)
	@chmod +x $(EXEC_DIR_DEMO)/loadmodules_omapl138_c6accel.sh
	@install -d $(EXEC_DIR_DEMO)
	

$(TARGET):	$(OBJFILES) $(C6ACCEL_LIB) $(XDC_LFILE)
	@echo
	@echo Linking $@ from $^..
	$(LINK.c) -o $@ $^ 

$(OBJFILES):	%.o: %.c $(HEADERS) $(XDC_CFLAGS)
	@echo Compiling $@ from $<..
	@echo $(XDC_CFLAGS)
	@echo $(XDC_LFILE)
	$(COMPILE.c) $(shell cat $(XDC_CFLAGS)) $(INCLUDES) -o $@ $<

clean:
	@echo Removing generated files..
	$(VERBOSE) -$(RM) -rf $(OBJFILES) $(TARGET) *~ *.d .dep
