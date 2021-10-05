################################################################################
#
# KMSGRAB
#
################################################################################

KMSGRAB_VERSION = 22916bdc63b133e52079a699ae6b2d25d990e798
KMSGRAB_SITE = $(call github,pcercuei,kmsgrab,$(KMSGRAB_VERSION))
KMSGRAB_LICENSE = MIT
KMSGRAB_DEPENDENCIES = libpng libdrm

$(eval $(cmake-package))
