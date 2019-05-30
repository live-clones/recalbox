PWD := $(shell pwd)
DL_DIR := $(if $(BR2_DL_DIR),$(BR2_DL_DIR),$(PWD)/dl)

.PHONY: merge buildBR

# So here is the trick : we try to "transmit" make parameters. We handle 2 cases :
# - no parameters: resync recalbox in Buildroot
# - parameters : the user is running a specific make commande like make sdl2-reconfigure or make clean, so don't sync
# - recalbox-*_defconfig: we MUST apply the patches.SOme of them change the dependencies, they must be done BEFORE we do make the defconfig itself
ifeq ($(MAKECMDGOALS),)
all: | merge buildBR
else
recalbox-%_defconfig: | merge
	BR2_DL_DIR=$(DL_DIR) $(MAKE) BR2_EXTERNAL="$(PWD)" BR2_PACKAGE_OVERRIDE_FILE="$(PWD)/local.mk" O="$(PWD)/output" -C "$(PWD)/buildroot" $(MAKEOVERRIDES) $@

%:
	BR2_DL_DIR=$(DL_DIR) $(MAKE) BR2_EXTERNAL="$(PWD)" BR2_PACKAGE_OVERRIDE_FILE="$(PWD)/local.mk" O="$(PWD)/output" -C "$(PWD)/buildroot" $(MAKEOVERRIDES) $@
endif


merge:
	CUSTOM_DIR=$(PWD)/custom BUILDROOT_DIR=$(PWD)/buildroot $(PWD)/scripts/linux/mergeToBR.sh

buildBR:
	@echo $(MAKEFLAGS) $(MAKECMDGOALS)
	BR2_DL_DIR=$(DL_DIR) $(MAKE) BR2_EXTERNAL="$(PWD)" BR2_PACKAGE_OVERRIDE_FILE="$(PWD)/local.mk" O="$(PWD)/output" -C "$(PWD)/buildroot" $(MAKEOVERRIDES) $(MAKECMDGOALS)
