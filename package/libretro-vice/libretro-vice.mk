################################################################################
#
# VICE
#
################################################################################
LIBRETRO_VICE_VERSION = 2bfda7bfdc7c3de7b5f4c31edfaf4bb1e8253fc8
LIBRETRO_VICE_SITE = $(call github,libretro,vice-libretro,$(LIBRETRO_VICE_VERSION))

define LIBRETRO_VICE_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" SHARED="$(TARGET_SHARED)" CXXFLAGS="$(TARGET_CXXFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(LIBRETRO_PLATFORM)"

endef

define LIBRETRO_VICE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/vice_x64_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/vice_x64_libretro.so
endef

define LIBRETRO_VICE_PRE_PATCH_FIXUP
	$(SED) 's/\r//g' $(@D)/libretro/libretro-core.c
endef

LIBRETRO_VICE_PRE_PATCH_HOOKS += LIBRETRO_VICE_PRE_PATCH_FIXUP


$(eval $(generic-package))
