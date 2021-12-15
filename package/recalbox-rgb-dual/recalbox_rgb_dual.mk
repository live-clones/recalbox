################################################################################
#
# RECALBOX_RGB_DUAL
#
################################################################################

RECALBOX_RGB_DUAL_VERSION = e1a38e1021edcf0627743bf2a99541211433dd05
RECALBOX_RGB_DUAL_SITE = $(call gitlab,recalbox,recalbox-rgb-dual,$(RECALBOX_RGB_DUAL_VERSION))
RECALBOX_RGB_DUAL_LICENSE = GPL
RECALBOX_RGB_DUAL_DEPENDENCIES = linux

define RECALBOX_RGB_DUAL_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define RECALBOX_RGB_DUAL_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
	$(HOST_DIR)/bin/linux-dtc $(@D)/recalboxrgbdual-overlay.dts -o $(@D)/recalboxrgbdual.dtbo
	$(INSTALL) -D -m 0644 $(@D)/recalboxrgbdual.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/recalboxrgbdual.dtbo
endef

$(eval $(kernel-module))
$(eval $(generic-package))
