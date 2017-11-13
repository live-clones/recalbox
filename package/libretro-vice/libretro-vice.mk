################################################################################
#
# VICE
#
################################################################################
LIBRETRO_VICE_VERSION = 8139feb92e5de44f7f5d5b6f9029a5edb52e4334
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
