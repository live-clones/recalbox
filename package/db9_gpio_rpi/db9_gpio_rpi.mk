################################################################################
#
# DB9_GPIO_RPI
#
################################################################################

DB9_GPIO_RPI_VERSION = c21faee29e42696722a953a8eee9093a223f1408
DB9_GPIO_RPI_SITE = $(call github,marqs85,db9_gpio_rpi,$(DB9_GPIO_RPI_VERSION))

DB9_GPIO_RPI_MODULE_SUBDIRS = db9_gpio_rpi-1.2
DB9_GPIO_RPI_MODULE_MAKE_OPTS = KERNELDIR=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
