################################################################################
#
# BEETLE_LYNX
#
################################################################################

LIBRETRO_BEETLE_LYNX_VERSION = 4756c2eb602afe8b607daaf224f03cbc5544b7a6
LIBRETRO_BEETLE_LYNX_SITE = $(call github,libretro,beetle-lynx-libretro,$(LIBRETRO_BEETLE_LYNX_VERSION))
LIBRETRO_BEETLE_LYNX_LICENSE = GPL-2.0
LIBRETRO_BEETLE_LYNX_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_BEETLE_LYNX_PLATFORM=rpi3
else
LIBRETRO_BEETLE_LYNX_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
endif

define LIBRETRO_BEETLE_LYNX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_BEETLE_LYNX_PLATFORM)"
endef

define LIBRETRO_BEETLE_LYNX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_lynx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_lynx_libretro.so
endef

$(eval $(generic-package))
