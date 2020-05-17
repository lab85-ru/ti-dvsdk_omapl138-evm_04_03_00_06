#
#  ======== Makefile ========
#

# Set location of various tools

# Import Tools Path from Rules.make
-include ../Rules.make

# set the location for package directory which contains the codec and
# server xdc packages
PACKAGE_DIR  = packages

# Set XDCPATH to contain necessary repositories.
# Please do not modify anything below
XDCPATH = $(CURDIR);$(C6ACCEL_INSTALL_DIR)/soc/packages;$(CODEC_INSTALL_DIR)/packages;$(XDAIS_INSTALL_DIR)/packages;$(FC_INSTALL_DIR)/packages;$(FC_INSTALL_DIR)/fctools/packages;$(BIOS_INSTALL_DIR)/packages;$(BIOSUTILS_INSTALL_DIR)/packages;$(CMEM_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR)/packages;$(LINK_INSTALL_DIR);$(CE_INSTALL_DIR)/packages;$(CE_INSTALL_DIR)/cetools/packages;

XDC = $(XDC_INSTALL_DIR)/xdc

# Conditionally, you can create verbose build output
XDCOPTIONS=v

export XDCOPTIONS
export XDCPATH
export XDCARGS
export CODEGEN_INSTALL_DIR

########################################################
## Shouldn't have to modify anything be low this line ##
########################################################

all: .all-packages
release: .release-packages

# This recursively locates all packages in the PACKAGE_DIR directory, builds
# and releases them
.all-packages: .check-XDC_INSTALL_DIR .check-CE_INSTALL_DIR .check-CODEC_INSTALL_DIR .check-C6ACCEL_INSTALL_DIR .check-FC_INSTALL_DIR .check-LINK_INSTALL_DIR .check-BIOS_INSTALL_DIR .check-CODEGEN_INSTALL_DIR .check-BIOSUTILS_INSTALL_DIR .check-XDAIS_INSTALL_DIR .check-CMEM_INSTALL_DIR .check-BIOS

	$(XDC) -PR $(C6ACCEL_INSTALL_DIR)/soc/packages/ti/c6accel
	$(XDC) -PR $(PACKAGE_DIR)	


.release-packages:
	$(XDC) release -PR $(PACKAGE_DIR)
	$(XDC) release -PR $(C6ACCEL_INSTALL_DIR)/soc/packages/ti/c6accel

install: 
	@echo Moved to Makefile.install

# This recursively cleans all packages in the "ti" directory
clean: clean-packages

clean-packages: .check-XDC_INSTALL_DIR .check-CE_INSTALL_DIR .check-CODEC_INSTALL_DIR .check-C6ACCEL_INSTALL_DIR .check-FC_INSTALL_DIR .check-LINK_INSTALL_DIR .check-BIOS_INSTALL_DIR .check-CODEGEN_INSTALL_DIR .check-BIOSUTILS_INSTALL_DIR .check-XDAIS_INSTALL_DIR .check-CMEM_INSTALL_DIR .check-BIOS
	$(XDC) clean -PR $(C6ACCEL_INSTALL_DIR)/soc/packages/ti/c6accel
	$(XDC) clean -PR $(PACKAGE_DIR)	


# This ensures that the BIOS you're using has been patched for SDSCM00004525.
# Specifically, the fix is to rename
# $(BIOS_INSTALL_DIR)/packages/ti/rtdx/package_export.xdc to package.xdc
.check-BIOS : $(BIOS_INSTALL_DIR)/packages/ti/rtdx/package.xdc

.check-CE_INSTALL_DIR:
ifndef CE_INSTALL_DIR
	$(error CE_INSTALL_DIR is not defined, Please refer to csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-CODEC_INSTALL_DIR:
ifndef CODEC_INSTALL_DIR
	$(error CODEC_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_##  and make sure the right package is installed)
endif

.check-C6ACCEL_INSTALL_DIR:
ifndef C6ACCEL_INSTALL_DIR
	$(error C6ACCEL_INSTALL_DIR is not defined)
endif

.check-FC_INSTALL_DIR:
ifndef FC_INSTALL_DIR
	$(error FC_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_##  and make sure the right package is installed)
endif

.check-LINK_INSTALL_DIR:
ifndef LINK_INSTALL_DIR
	$(error LINK_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_##  and make sure the right package is installed)
endif

.check-BIOS_INSTALL_DIR:
ifndef BIOS_INSTALL_DIR
	$(error BIOS_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-XDC_INSTALL_DIR:
ifndef XDC_INSTALL_DIR
	$(error XDC_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-CODEGEN_INSTALL_DIR:
ifndef CODEGEN_INSTALL_DIR
	$(error CODEGEN_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-BIOSUTILS_INSTALL_DIR:
ifndef BIOSUTILS_INSTALL_DIR
	$(error BIOSUTILS_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-XDAIS_INSTALL_DIR:
ifndef XDAIS_INSTALL_DIR
	$(error XDAIS_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_## and make sure the right package is installed)
endif

.check-CMEM_INSTALL_DIR:
ifndef CMEM_INSTALL_DIR
	$(error CMEM_INSTALL_DIR is not defined, Please refer csRules.mak under cs1omapl138_1_00_##  and make sure the right package is installed)
endif


$(BIOS_INSTALL_DIR)/packages/ti/rtdx/package.xdc :
	$(warning Your BIOS needs to be patched for SDSCM00004525!)
	$(error Please copy $(BIOS_INSTALL_DIR)/packages/ti/rtdx/package_export.xdc to $@)
