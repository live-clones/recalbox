################################################################################
#
# LIBRGA
#
################################################################################

LIBRGA_VERSION = 1fc02d56d97041c86f01bc1284b7971c6098c5fb
LIBRGA_SITE = $(call github,rockchip-linux,linux-rga,$(LIBRGA_VERSION))
LIBRGA_LICENSE = GPL-2+
LIBRGA_LICENSE_FILES = COPYING

LIBRGA_INSTALL_STAGING = YES

LIBRGA_DEPENDENCIES = libdrm

$(eval $(meson-package))
