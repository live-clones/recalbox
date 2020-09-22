################################################################################
#
# GAMECON_GPIO_RPI
#
################################################################################

GAMECON_GPIO_RPI_VERSION = d70ac2459443f2bd96e8d99bf5dd5b84658ffe77
GAMECON_GPIO_RPI_SITE = $(call github,marqs85,gamecon_gpio_rpi,$(GAMECON_GPIO_RPI_VERSION))

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
