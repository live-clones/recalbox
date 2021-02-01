################################################################################
#
# GAMECON_GPIO_RPI
#
################################################################################

GAMECON_GPIO_RPI_VERSION = v1.4.0-rb
GAMECON_GPIO_RPI_SITE = https://gitlab.com/recalbox/gamecon_gpio_rpi.git
GAMECON_GPIO_RPI_SITE_METHOD = git
GAMECON_GPIO_RPI_LICENSE = GPL-2.0

GAMECON_GPIO_RPI_DEPENDENCIES = linux

define GAMECON_GPIO_RPI_COPY_CMDS
	cp -r $(@D)/gamecon_gpio_rpi-1.4/* $(@D)/.
endef

GAMECON_GPIO_RPI_CONFIGURE_CMDS += $(GAMECON_GPIO_RPI_COPY_CMDS)

define GAMECON_GPIO_RPI_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define GAMECON_GPIO_RPI_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
endef

$(eval $(kernel-module))
$(eval $(generic-package))
