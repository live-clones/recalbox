################################################################################
#
# BEETLE_LYNX
#
################################################################################
LIBRETRO_BEETLE_LYNX_VERSION = ad4347a58e8334bb351abaf82cb7bcb08d7e3c8c
LIBRETRO_BEETLE_LYNX_SITE = $(call github,libretro,beetle-lynx-libretro,$(LIBRETRO_BEETLE_LYNX_VERSION))

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
	LIBRETRO_BEETLE_LYNX_PLATFORM=rpi3
else
	LIBRETRO_BEETLE_LYNX_PLATFORM=$(LIBRETRO_PLATFORM)
endif

define LIBRETRO_BEETLE_LYNX_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_BEETLE_LYNX_PLATFORM)"
endef

define LIBRETRO_BEETLE_LYNX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_lynx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_lynx_libretro.so
endef

$(eval $(generic-package))
