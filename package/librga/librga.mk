################################################################################
#
# LIBRGA
#
################################################################################

LIBRGA_VERSION = 2134dde256e10bf54c490d06f44f22a6859c83eb
LIBRGA_SITE = $(call github,rockchip-linux,linux-rga,$(LIBRGA_VERSION))
LIBRGA_LICENSE = GPL-2+
LIBRGA_LICENSE_FILES = COPYING

LIBRGA_INSTALL_STAGING = YES

LIBRGA_DEPENDENCIES = libdrm

$(eval $(meson-package))
