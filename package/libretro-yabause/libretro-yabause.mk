################################################################################
#
# YABAUSE
#
################################################################################

LIBRETRO_YABAUSE_VERSION = f515e5a49336195c61b751e539aadae72fbe0aa7
LIBRETRO_YABAUSE_SITE = $(call github,libretro,yabause,$(LIBRETRO_YABAUSE_VERSION))
LIBRETRO_YABAUSE_LICENSE = GPL-2.0

define LIBRETRO_YABAUSE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/yabause/src/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/yabause/src/libretro -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_YABAUSE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/yabause/src/libretro/yabause_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/yabause_libretro.so
endef

$(eval $(generic-package))
