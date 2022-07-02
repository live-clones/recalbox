################################################################################
#
# LIBRGA
#
################################################################################

LIBRGA_VERSION = 16f547acaf4af082ef118e7da10d198458be0602
LIBRGA_SITE = $(call github,hardkernel,rk3568_linux_linux-rga,$(LIBRGA_VERSION))
LIBRGA_LICENSE = GPL-2+
LIBRGA_LICENSE_FILES = COPYING

LIBRGA_INSTALL_STAGING = YES

LIBRGA_DEPENDENCIES = libdrm

$(eval $(meson-package))
