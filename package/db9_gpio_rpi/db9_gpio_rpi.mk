################################################################################
#
# DB9_GPIO_RPI
#
################################################################################

DB9_GPIO_RPI_VERSION = v1.3.1
DB9_GPIO_RPI_SITE = https://gitlab.com/recalbox/db9_gpio_rpi.git
DB9_GPIO_RPI_LICENSE = COPYRIGHT

DB9_GPIO_RPI_SITE_METHOD = git

DB9_GPIO_RPI_MODULE_SUBDIRS = db9_gpio_rpi-1.2
DB9_GPIO_RPI_MODULE_MAKE_OPTS = KERNELDIR=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
