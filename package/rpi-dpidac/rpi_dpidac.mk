################################################################################
#
# RPI_DPIDAC
#
################################################################################

RPI_DPIDAC_VERSION = c42845966a6dca0e0323f9bb7ae308c56736de55
RPI_DPIDAC_SITE = $(call github,osvathrobi,rpi-dpidac,$(RPI_DPIDAC_VERSION))
RPI_DPIDAC_LICENSE = GPL
RPI_DPIDAC_DEPENDENCIES = linux

define RPI_DPIDAC_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define RPI_DPIDAC_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
	$(INSTALL) -D -m 0644 $(@D)/vc4-vga666.dtbo $(BINARIES_DIR)/rpi-firmware/overlays/vc4-vga666.dtbo
	#$(INSTALL) -D -m 0644 $(@D)/vc4-vga666-overlay.dts $(BINARIES_DIR)/rpi-firmware/overlays/vc4-vga666-overlay.dts
endef

$(eval $(kernel-module))
$(eval $(generic-package))
