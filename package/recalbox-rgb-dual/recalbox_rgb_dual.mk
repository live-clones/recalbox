################################################################################
#
# RECALBOX_RGB_DUAL
#
################################################################################

RECALBOX_RGB_DUAL_VERSION = 2aedbe30ef6242298c6696b3c4b5d0b612f09237
RECALBOX_RGB_DUAL_SITE = $(call gitlab,recalbox,recalbox-rgb-dual,$(RECALBOX_RGB_DUAL_VERSION))
RECALBOX_RGB_DUAL_LICENSE = GPL
RECALBOX_RGB_DUAL_DEPENDENCIES = linux

define RECALBOX_RGB_DUAL_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define RECALBOX_RGB_DUAL_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
	$(HOST_DIR)/bin/linux-dtc $(@D)/recalboxrgbdual-overlay.dts -o $(@D)/recalboxrgbdual.dtbo
	$(HOST_DIR)/bin/linux-dtc $(@D)/recalboxrgbdual-thirdparty-overlay.dts -o $(@D)/recalboxrgbdual-thirdparty.dtbo
	$(INSTALL) -D -m 0644 $(@D)/recalboxrgbdual.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/recalboxrgbdual.dtbo
	$(INSTALL) -D -m 0644 $(@D)/recalboxrgbdual-thirdparty.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/recalboxrgbdual-thirdparty.dtbo
endef

$(eval $(kernel-module))
$(eval $(generic-package))
