################################################################################
#
# XPI_GAMECON_RPI Recalbox Team 2020
#
################################################################################

XPI_GAMECON_RPI_VERSION = v0.1
XPI_GAMECON_RPI_SITE = https://gitlab.com/recalbox/packages/hardware/xpi_gamecon_rpi.git
XPI_GAMECON_RPI_SITE_METHOD = git
XPI_GAMECON_RPI_DEPENDENCIES = linux
XPI_GAMECON_RPI_LICENSE = GPL-3.0

define XPI_GAMECON_RPI_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define XPI_GAMECON_RPI_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
	$(INSTALL) -d -m 0755 $(TARGET_DIR)/home/pi
	cp -r $(XPI_GAMECON_RPI_PKGDIR)/ressources/osd $(TARGET_DIR)/home/pi
endef

$(eval $(kernel-module))
$(eval $(generic-package))
