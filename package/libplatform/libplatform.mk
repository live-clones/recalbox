################################################################################
#
# libplatform
#
################################################################################

LIBPLATFORM_VERSION = 2d90f98620e25f47702c9e848380c0d93f29462b
LIBPLATFORM_SITE = $(call github,Pulse-Eight,platform,$(LIBPLATFORM_VERSION))
LIBPLATFORM_LICENSE = GPLv2+, PHP license v3.01
LIBPLATFORM_LICENSE_FILES = src/os.h src/util/fstrcmp.c
LIBPLATFORM_INSTALL_STAGING = YES

$(eval $(cmake-package))
