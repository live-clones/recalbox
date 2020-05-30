################################################################################
#
# DB9_GPIO_RPI
#
################################################################################

DB9_GPIO_RPI_VERSION = 6f257ff226b866a37b0ea6d6f0a7f91b5b73c421
DB9_GPIO_RPI_SITE = $(call github,marqs85,db9_gpio_rpi,$(DB9_GPIO_RPI_VERSION))

DB9_GPIO_RPI_DEPENDENCIES = linux
DB9_GPIO_RPI_STRIP_COMPONENTS = 1

define DB9_GPIO_RPI_COPY_CMDS
	cp -r $(@D)/db9_gpio_rpi-1.2/* $(@D)/.
endef

DB9_GPIO_RPI_CONFIGURE_CMDS += $(DB9_GPIO_RPI_COPY_CMDS)

define DB9_GPIO_RPI_BUILD_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR)
endef

define DB9_GPIO_RPI_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) $(LINUX_MAKE_FLAGS) KERNELDIR=$(LINUX_DIR) modules_install
endef

$(eval $(generic-package))
